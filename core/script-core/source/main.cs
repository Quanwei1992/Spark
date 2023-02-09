

using System;
using System.Dynamic;
using System.Runtime.CompilerServices;

namespace Spark
{

	public struct Vector3
	{
		public float X, Y, Z;

		public Vector3(float x, float y, float z)
		{
			X = x; Y = y; Z = z;
		}
	}

	public class Entity
	{
		public float FloatVar { get; set; }

		public Entity()
		{
			Console.WriteLine("Main constructor");
			Log("sxli",1121);

			Vector3 pos = new Vector3(5,2,1);
			Vector3 result = Log(pos);
			Console.WriteLine($"{result.X}, {result.Y}, {result.Z}");

		}

		private void Log(string text, int parameter)
		{
			InternalCalls.NativeLog(text,parameter);
		}

		private Vector3 Log(Vector3 pos)
		{
			InternalCalls.NativeLog_Vector(ref pos,out Vector3 result);
			return result;
		}

		public void PrintMessage()
		{
			Console.WriteLine("Hello World from C#");
		}
		public void PrintInt(int value)
		{
			Console.WriteLine($"C# says: {value}");
		}
		public void PrintCustomMessage(string message)
		{
			Console.WriteLine($"C# says: {message}");
		}
	}

}
