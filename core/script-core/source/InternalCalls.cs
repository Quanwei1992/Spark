using System;
using System.Runtime.CompilerServices;

namespace Spark
{
	public static class InternalCalls
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_GetTranslation(UInt32 entityID,out Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_SetTranslation(UInt32 entityID, ref Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyPressed(KeyCode keycode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsMouseButtonPressed(MouseCode button);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Input_GetMousePosition(out float x, out float y);
	}
}
