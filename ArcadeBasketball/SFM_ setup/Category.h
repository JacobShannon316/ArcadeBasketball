#pragma once
namespace Category
{
	enum Type
	{
		None					= 0,
		SceneAirLayer			= 1 << 0,
		PlayerAircraft			= 1 << 1,
		AlliedAircraft			= 1 << 2,
		EnemyAircraft			= 1 << 3,
		Pickup					= 1 << 4,
		AlliedProjectile		= 1 << 5,
		EnemyProjectile			= 1 << 6,
		ParticleSystem			= 1 << 7,
		SoundEffect				= 1 << 8,
		Ball					= 1 << 9,
		Player					= 1 << 10,
		Hoop					= 1 << 11,
		AI						= 1 << 12,
		Head					= 1 << 13,
		Actor,

		Aircraft = PlayerAircraft | AlliedAircraft | EnemyAircraft,
		Projectile = AlliedProjectile | EnemyProjectile,
	};
}

