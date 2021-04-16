#pragma once
// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
	class Shader;
	class SoundBuffer;
}

namespace Textures
{
	enum ID
	{
		Entities,
		TitleScreen,
		CharacterSelectionScreen,
		Buttons,

		CowHeadButton,
		AlejandroHeadButton,
		AnastasiiaHeadButton,
		JoshHeadButton,

		CowHeadButtonSelection,
		AlejandroHeadButtonSelection,
		AnastasiiaHeadButtonSelection,
		JoshHeadButtonSelection,
		JacobHeadButton,

		Background,
		Court,
		Hoop,
		Ball,
		RedTeam,
		BlueTeam,

		Head1,
		Head2,
		Head3,
		Head4,

		Hero2,
	};
}

namespace Shaders
{
	enum ID
	{
		BrightnessPass,
		DownSamplePass,
		GaussianBlurPass,
		AddPass,
	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}

namespace SoundEffect
{
	enum ID
	{
		Swish,
		Rim,
		Bouncing,
		AlliedGunfire,
		EnemyGunfire,
		Explosion1,
		Explosion2,
		LaunchMissile,
		CollectPickup,
		Button,
	};
}

namespace Music
{
	enum ID
	{
		MenuTheme,
		MissionTheme,
	};
}


// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID>			TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>					FontHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID>				ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect::ID>	SoundBufferHolder;

