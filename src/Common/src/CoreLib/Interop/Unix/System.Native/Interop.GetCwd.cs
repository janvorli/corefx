// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#nullable enable
using System;
using System.Buffers;
using System.Runtime.InteropServices;

internal static partial class Interop
{
    internal static partial class Sys
    {
        [DllImport(Libraries.SystemNative, EntryPoint = "SystemNative_GetCwd", SetLastError = true)]
        private static extern unsafe byte* GetCwd(byte* buffer, int bufferLength);

        internal static unsafe string GetCwd()
        {
            const int StackLimit = 256;

            // First try to get the path into a buffer on the stack
            byte* stackBuf = stackalloc byte[StackLimit];
            string? result = GetCwdHelper(stackBuf, StackLimit);
            if (result != null)
            {
                return result;
            }

            // If that was too small, try increasing large buffer sizes
            int bufferSize = StackLimit;
            do
            {
                checked { bufferSize *= 2; }
                byte[] buf = ArrayPool<byte>.Shared.Rent(bufferSize);
                try
                {
                    fixed (byte* ptr = &buf[0])
                    {
                        result = GetCwdHelper(ptr, buf.Length);
                        if (result != null)
                        {
                            return result;
                        }
                    }
                }
                finally
                {
                    ArrayPool<byte>.Shared.Return(buf);
                }
            }
            while (true);
        }

        private static unsafe string? GetCwdHelper(byte* ptr, int bufferSize)
        {
            // Call the real getcwd
            byte* result = GetCwd(ptr, bufferSize);

            // If it returned non-null, the null-terminated path is in the buffer
            if (result != null)
            {
                return Marshal.PtrToStringAnsi((IntPtr)ptr);
            }

            // Otherwise, if it failed due to the buffer being too small, return null;
            // for anything else, throw.
            ErrorInfo errorInfo = Interop.Sys.GetLastErrorInfo();
            if (errorInfo.Error == Interop.Error.ERANGE)
            {
               return null;
            }
            throw Interop.GetExceptionForIoErrno(errorInfo);
        }
    }
}
