using System.Runtime.CompilerServices;

namespace Spark
{
	public static class InternalCalls
	{

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void NativeLog(string text, int parameter);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void NativeLog_Vector(ref Vector3 parameter, out Vector3 result);
	}
}
