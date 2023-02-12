using System;

namespace Spark
{

	public struct Vector3
	{
		public float X,Y,Z;

		public Vector3(float scalar)
		{
			X = scalar; Y = scalar; Z = scalar;
		}
		public Vector3(float x, float y, float z)
		{
			X = x; Y = y; Z = z;
		}

		public static Vector3 operator*(Vector3 vector, float scalar)
		{
			return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
		}
		public static Vector3 operator+(Vector3 a,Vector3 b)
		{
			return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
		}

		public static Vector3 Zero => new Vector3(0);
	}

	public struct Vector2
	{
		public float X, Y;

		public Vector2(float scalar)
		{
			X = scalar; Y = scalar;
		}
		public Vector2(float x, float y)
		{
			X = x; Y = y;
		}

		public static Vector2 operator *(Vector2 vector, float scalar)
		{
			return new Vector2(vector.X * scalar, vector.Y * scalar);
		}

		public static Vector2 Zero => new Vector2(0);
	}

	public class Entity
	{
		public readonly UInt32 Id = 0;

		protected Entity()
		{
			Id = 0;
		}

		internal Entity(UInt32 id)
		{
			Id = id;
		}

		protected virtual void OnCreate()
		{
		}

		protected virtual void OnUpdate(float ts)
		{
		}

		public Vector3 Translation
		{
			get
			{
				InternalCalls.Entity_GetTranslation(Id, out Vector3 translation);
				return translation;
			}
			set => InternalCalls.Entity_SetTranslation(Id, ref value);
		}

	}
}
