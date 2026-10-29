#include "pch.hpp"
#include "config.hpp"

namespace
{
	constexpr std::uint32_t k_archive_magic = 0x52454C43;		// "CLER"
	constexpr std::uint32_t k_archive_version = 1;
}

void c_config::run( std::string name )
{
	if ( PWSTR appdata_path; SUCCEEDED( SHGetKnownFolderPath( FOLDERID_RoamingAppData, 0, nullptr, &appdata_path ) ) )
	{
		path = appdata_path;
		path /= name;

		CoTaskMemFree( appdata_path );
	}

	if ( std::filesystem::is_directory( path ) )
	{
		std::transform
		(
			std::filesystem::directory_iterator { path },
			std::filesystem::directory_iterator { },
			std::back_inserter( configs ),
			[]( const auto& entry ) { return entry.path().filename().string(); }
		);
	}
}

void c_config::load( size_t id )
{
	if ( !std::filesystem::is_directory( path ) )
	{
		std::filesystem::remove( path );
		std::filesystem::create_directory( path );
	}

	if ( id >= configs.size() )
		return;

	std::ifstream in { path / configs[id], std::ios::binary };

	if ( !in.good() )
		return;

	auto archive = archivex<std::ifstream>{ in };

	std::uint32_t magic {}, version {}, size {};
	archive >> magic >> version >> size;

	if ( !in.good() || magic != k_archive_magic || version != k_archive_version || size != sizeof( clicker ) )
	{
		log_warn( "Invalid or outdated config, resetting to defaults." );
		clicker = { };
		in.close();
		return;
	}

	archive >> clicker;
	in.close();
}

void c_config::save( size_t id ) const
{
	if ( !std::filesystem::is_directory( path ) )
	{
		std::filesystem::remove( path );
		std::filesystem::create_directory( path );
	}

	if ( id >= configs.size() )
		return;

	std::ofstream out { path / configs[id], std::ios::binary };

	if ( !out.good() )
		return;

	auto archive = archivex<std::ofstream>{ out };

	archive << k_archive_magic << k_archive_version << (std::uint32_t) sizeof( clicker );
	archive << clicker;

	out.close();
}

void c_config::add( std::string name )
{
	if ( !( name.empty() ) && std::find( std::cbegin( configs ), std::cend( configs ), name ) == std::cend( configs ) )
		configs.emplace_back( name );
}

void c_config::remove( size_t id )
{
	if ( id >= configs.size() )
		return;

	std::filesystem::remove( path / configs[id] );
	configs.erase( configs.cbegin() + id );
}

void c_config::rename( size_t item, std::string new_name )
{
	if ( item >= configs.size() )
		return;

	std::filesystem::rename( path / configs[item], path / new_name );
	configs[item] = new_name;
}

void c_config::reset()
{
	clicker = { };
}
