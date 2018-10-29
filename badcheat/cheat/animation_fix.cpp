#include "animation_fix.h"

DECLARE_SOURCE_EXTERN(c_animation_correct, animation_fix);

//	est. effort for recreating this animation fix: ~200 lines

void c_animation_correct::fix_player_animations( c_player* player ) {
	
	static float m_old_simtime[64];
	static std::array<float, 24> m_poses[64];
	static animation_layer m_layers[64][15];

	//auto _old_time = game->m_globals( )->m_time;
	//auto _old_frametime = game->m_globals( )->m_frametime;
	//game->m_globals( )->m_time = player->simulation_time( );
	//game->m_globals( )->m_frametime = game->m_globals( )->m_tickinterval;

	if ( m_old_simtime[ player->index( ) ] != player->simulation_time( ) ) {
		player->client_animation() = true;

		player->update_animation();

		m_poses[ player->index( ) ] = player->pose_parameters( );
		std::memcpy( m_layers[ player->index( ) ], player->anim_overlay( ), ( sizeof( animation_layer ) * player->anim_layer_count( ) ) );

		m_old_simtime[ player->index( ) ] = player->simulation_time( );
	}
	else
		player->client_animation( ) = false;

	if (player == game->m_local( ))
		game->m_local( )->set_abs_angles( vec3( 0, game->m_local( )->animstate( )->m_flGoalFeetYaw, 0 ) );

	//game->m_globals( )->m_time = _old_time;
	//game->m_globals( )->m_frametime = _old_frametime;
	player->pose_parameters( ) = m_poses[ player->index( ) ];
	std::memcpy(player->anim_overlay( ), m_layers[ player->index( ) ], ( sizeof( animation_layer ) * player->anim_layer_count( ) ) );
}

void c_animation_correct::release( c_player* player ) {
	//	for animstate::update recreation
}