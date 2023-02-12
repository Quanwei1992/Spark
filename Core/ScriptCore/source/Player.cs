
using System;
using Spark;

namespace Sandbox
{
	public class Player : Spark.Entity
	{
		public float Speed = 2.0f;
		protected override void OnCreate()
		{
			Console.WriteLine($"Player OnCreate - {Id}");
		}

		protected override void OnUpdate(float ts)
		{
			var translation = Translation;

			Vector3 velocity = Vector3.Zero;
			if (Input.IsKeyPressed(KeyCode.W)) { velocity.Y = 1.0f; }
			else if(Input.IsKeyPressed(KeyCode.S)) { velocity.Y = -1.0f; }
			else if(Input.IsKeyPressed(KeyCode.D)) { velocity.X = 1.0f; }
			else if(Input.IsKeyPressed(KeyCode.A)) { velocity.X = -1.0f; }

			translation += velocity * Speed * ts;
			Translation = translation;
		}

	}
}
