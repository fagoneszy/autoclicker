#include "pch.hpp"
#include "menu.hpp"

namespace directx9
{
	inline IDirect3D9* context = {};
	inline IDirect3DDevice9* device = {};
	inline D3DPRESENT_PARAMETERS param;
}

namespace ctx 
{
	inline HWND hWnd = {};
	inline WNDCLASSEX wc = {};
	inline int menu_width = { 600 }, menu_height = { 350 };

	inline ImGuiStyle* imgui_style = {};
	inline ImGuiIO* imgui_io = {};
}

bool menu_wants_text_input()
{
	return ImGui::GetCurrentContext() && ImGui::GetIO().WantTextInput;
}

void c_menu::on_paint()
{
	static int x = 0, y = 0;
	static bool b_shown = true;

	const bool b_want_visible = vars::b_menu_open.load() && !vars::key::b_hide_forced.load();

	if ( b_want_visible != b_shown )
	{
		ShowWindow( ctx::hWnd, b_want_visible ? SW_SHOW : SW_HIDE );

		if ( b_want_visible )
		{
			SetWindowPos( ctx::hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
			SetForegroundWindow( ctx::hWnd );
		}
		else if ( const auto h_game = FindWindowW( L"LWJGL", nullptr ) )
		{
			SetForegroundWindow( h_game );
		}

		b_shown = b_want_visible;
	}

	if ( !vars::b_is_running )
		std::exit( 0 );

	vars::key::b_binding_key.store( false );

	ImGui::SetNextWindowSize( { (float)ctx::menu_width, (float)ctx::menu_height }, ImGuiCond_Once );
	ImGui::SetNextWindowPos( { 0, 0 }, ImGuiCond_Once );

	if ( ImGui::Begin( "Clicker 1.8.9", &vars::b_is_running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove ) )
	{
		if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
			get_mouse_offset( x, y, ctx::hWnd);

		if ( y >= 0 && y <= ( ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 4 ) && ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
			set_position( x, y, ctx::menu_width, ctx::menu_height, ctx::hWnd);

		if ( ImGui::BeginTabBar( "##var::clicker::tabs" ) )
		{
			if ( ImGui::BeginTabItem( "mouse" ) )
			{
				ImGui::Text( "Chave mestra" );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Liga/desliga o clicker.\nSempre: sempre ativo (tecla ignorada).\nSegurar: ativo enquanto a tecla estiver pressionada.\nAlternar: liga/desliga ao soltar a tecla.\nSem tecla atribuída, Segurar/Alternar ficam desativados." );

				ImGui::Separator();

				keybind_button( config.clicker.i_clicker_key, 150, 22 );

				ImGui::SameLine();

				ImGui::PushItemWidth( 110.f );
				ImGui::Combo( "##var::clicker::i_key_type", &config.clicker.i_key_type, "Sempre\0Segurar\0Alternar\0\0" );
				ImGui::PopItemWidth();

				ImGui::Text( "Botão esquerdo" );
				ImGui::Separator();

				ImGui::Checkbox( "Ativar clicker esquerdo", &config.clicker.b_enable_left_clicker );

				if ( config.clicker.b_enable_left_clicker )
				{
					ImGui::Text( "Ativação" );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Tecla/botão que dispara o clique esquerdo. Aceita qualquer tecla, incluindo laterais do mouse (Mouse 4/5).\nSegurar = só clica enquanto estiver pressionado e para ao soltar." );

					keybind_button( config.clicker.i_left_key, 150, 22 );

					ImGui::SameLine();

					ImGui::PushItemWidth( 110.f );
					ImGui::Combo( "##var::clicker::i_left_mode", &config.clicker.i_left_mode, "Sempre\0Segurar\0Alternar\0\0" );
					ImGui::PopItemWidth();

					ImGui::Text( "CPS esquerdo" );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Ctrl + clique esquerdo no slider permite digitar um valor exato.\nIntervalo mínimo entre cliques: 50 ms (o Minecraft não registra mais rápido).\nSe o CPS pedir intervalo < 50 ms, aplica o piso e manda um clique extra\n(double click) para manter a média — teto registrável ~20 CPS.\nValores entre 9.5 e 12.5 são recomendados para anti-cheats." );

					ImGui::SliderFloat( "##var::clicker::f_left_cps", &config.clicker.f_left_cps, 1.f, 30.f, "%.2f cps" );
				}

				ImGui::Text( "Botão direito" );
				ImGui::Separator();

				ImGui::Checkbox( "Ativar clicker direito", &config.clicker.b_enable_right_clicker );

				if ( config.clicker.b_enable_right_clicker )
				{
					ImGui::Text( "Ativação" );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Tecla/botão que dispara o clique direito. Aceita qualquer tecla, incluindo laterais do mouse (Mouse 4/5).\nSegurar = só clica enquanto estiver pressionado e para ao soltar." );

					keybind_button( config.clicker.i_right_key, 150, 22 );

					ImGui::SameLine();

					ImGui::PushItemWidth( 110.f );
					ImGui::Combo( "##var::clicker::i_right_mode", &config.clicker.i_right_mode, "Sempre\0Segurar\0Alternar\0\0" );
					ImGui::PopItemWidth();

					ImGui::Text( "CPS direito" );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Ctrl + clique esquerdo no slider permite digitar um valor exato.\nValores entre 9.5 e 12.5 são recomendados para anti-cheats." );

					ImGui::SliderFloat( "##var::clicker::f_right_cps", &config.clicker.f_right_cps, 1.f, 30.f, "%.2f cps" );
				}

				ImGui::Separator();
				ImGui::Checkbox( "Não clicar à esquerda com o botão direito pressionado", &config.clicker.b_left_skip_while_right );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Mantém o comportamento original: o clique esquerdo só roda com o botão direito solto." );

				ImGui::Text( "Foco" );
				ImGui::Separator();

				ImGui::PushItemWidth( 140.f );
				ImGui::Combo( "##var::clicker::i_version_type", &config.clicker.i_version_type, "Minecraft\0Personalizado\0\0" );
				ImGui::PopItemWidth();

				switch ( config.clicker.i_version_type )
				{
					case 0:
							ImGui::Checkbox( "Trabalhar no inventário", &config.clicker.b_work_in_inventory );
							if ( ImGui::IsItemHovered() )
								ImGui::SetTooltip( "Se ativo, o clicker também funciona com o inventário aberto." );
						break;
					case 1:
						ImGui::InputText( "##var::input::buffer", config.clicker.str_window_title, IM_ARRAYSIZE( config.clicker.str_window_title ) );
						if ( ImGui::IsItemHovered() )
							ImGui::SetTooltip( "Se ficar em branco, funciona em qualquer lugar." );
						break;
				}

				ImGui::Text( "Diagnóstico" );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Condições em tempo real. O clique só acontece no jogo quando\ntodas as marcadas com " ICON_FA_CHECK " estão satisfeitas." );

				ImGui::Separator();

				const auto gate = []( const char* sz_label, bool b_ok )
				{
					ImGui::TextColored(
						b_ok ? ImVec4( 0.35f, 0.85f, 0.35f, 1.f ) : ImVec4( 0.85f, 0.35f, 0.35f, 0.85f ),
						"%s %s", b_ok ? ICON_FA_CHECK : ICON_FA_TIMES, sz_label );
				};

				if ( config.clicker.i_version_type == 0 )
				{
					const bool b_found = FindWindowW( L"LWJGL", nullptr ) || FindWindowW( L"GLFW30", nullptr );
					gate( "Janela do jogo encontrada", b_found );
				}

				gate( "Chave mestra ativa", vars::key::b_master_active.load() );

				if ( config.clicker.b_enable_left_clicker )
				{
					const bool b_key_down = config.clicker.i_left_key && ( ( GetAsyncKeyState( config.clicker.i_left_key ) & 0x8000 ) != 0 );
					gate( "Ativação esquerda (tecla/botão)", ( config.clicker.i_left_mode == 0 ) || b_key_down );
				}

				if ( config.clicker.b_enable_right_clicker )
				{
					const bool b_key_down = config.clicker.i_right_key && ( ( GetAsyncKeyState( config.clicker.i_right_key ) & 0x8000 ) != 0 );
					gate( "Ativação direita (tecla/botão)", ( config.clicker.i_right_mode == 0 ) || b_key_down );
				}

				gate( "Minecraft em foco", vars::key::b_game_focused.load() );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "O MC precisa estar em primeiro plano.\nEnquanto esta janela do menu estiver focada o indicador\nfica em " ICON_FA_TIMES " — é esperado, clique no mundo do MC." );

				gate( "Cursor sem GUI (no jogo)", !vars::key::b_cursor_visible.load() );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "" ICON_FA_CHECK " = mouse capturado pelo jogo (fora de menus).\n" ICON_FA_TIMES " = cursor visível (inventário, chat, opções ou\na seta do Windows com o menu em foco)." );

				gate( "Menu fechado (sem pausa)", !( config.clicker.b_pause_when_menu_open && vars::b_menu_open.load() ) );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Com a opção 'Pausar cliques com o menu aberto' marcada,\no clique fica bloqueado enquanto esta interface está visível\n(desmarque para testar com o menu aberto)." );

				gate( "Clicando à esquerda", vars::stats::b_left_active.load() );
				gate( "Clicando à direita", vars::stats::b_right_active.load() );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Só fica " ICON_FA_CHECK " quando todos os gates acima estão " ICON_FA_CHECK "\ne a tecla de ativação correspondente está pressionada." );

				if ( !config.clicker.b_enable_blatant )
				{
					ImGui::Text( "Aleatorização" );
					ImGui::Separator();

					ImGui::Checkbox( "Persistência", &config.clicker.b_enable_persistence );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Randomização aplicada em curto período em cima do CPS definido.\nRecomendado." );

					if ( config.clicker.b_enable_persistence )
						ImGui::SliderFloat( "##var::clicker::f_persistence_value", &config.clicker.f_persistence_value, 1.f, 5.f, "%.1f cps" );

					ImGui::Checkbox( "Chance de picos", &config.clicker.b_enable_cps_spikes );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Aumenta o CPS para o valor definido com a chance informada." );

					if ( config.clicker.b_enable_cps_spikes )
					{
						ImGui::SliderInt( "##var::clicker::i_cps_spike_chance", &config.clicker.i_cps_spike_chance, 1, 100, "chance %d%%" );
						ImGui::SliderFloat( "##var::clicker::f_cps_spike_add", &config.clicker.f_cps_spike_add, 1.f, 5.f, "add %.1f cps" );
					}

					ImGui::Checkbox( "Chance de quedas", &config.clicker.b_enable_cps_drops );
					if ( ImGui::IsItemHovered() )
						ImGui::SetTooltip( "Reduz o CPS para o valor definido com a chance informada." );

					if ( config.clicker.b_enable_cps_drops )
					{
						ImGui::SliderInt( "##var::clicker::i_cps_drop_chance", &config.clicker.i_cps_drop_chance, 1, 100, "chance %d%%" );
						ImGui::SliderFloat( "##var::clicker::f_cps_drop_remove", &config.clicker.f_cps_drop_remove, 1.f, 5.f, "remove %.1f cps" );
					}

					ImGui::Checkbox( "Opções avançadas", &config.clicker.b_enable_advanced_options );
					if ( config.clicker.b_enable_advanced_options )
					{
						ImGui::Text( "Atraso máximo de atualização" );
						if ( ImGui::IsItemHovered() )
							ImGui::SetTooltip( "Atraso máximo de atualização do CPS." );

						ImGui::SliderFloat( "##var::clicker::f_persistence_update_rate", &config.clicker.f_persistence_update_rate, 1500.f, 10000.f, "%.1f ms" );

						ImGui::Text( "Randomização do timer padrão" );
						if ( ImGui::IsItemHovered() )
							ImGui::SetTooltip( "Variação padrão do delay do timer." );

						ImGui::SliderFloat( "##var::clicker::f_default_timer_randomization", &config.clicker.f_default_timer_randomization, 1.f, 15.f, "%.1f ms" );
					}
				}

				ImGui::Separator();
				ImGui::Checkbox( "Explícito (sem aleatorização)", &config.clicker.b_enable_blatant );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Se ativo nenhuma randomização é adicionada. Use por sua conta e risco." );

				ImGui::EndTabItem();
			}

			if ( ImGui::BeginTabItem( "menu" ) )
			{
				ImGui::Text( "Tecla do menu" );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Abre/fecha esta interface (padrão: M).\nAceita qualquer tecla ou botão do mouse, incluindo laterais (Mouse 4/5).\nAlternar = abre e fecha a cada clique; Segurar = aberto só enquanto a tecla estiver pressionada." );

				ImGui::Separator();

				keybind_button( config.clicker.i_menu_key, 150, 22 );

				ImGui::SameLine();

				int i_menu_mode = ( config.clicker.i_menu_mode == 1 ) ? 1 : 0;
				ImGui::PushItemWidth( 110.f );
				if ( ImGui::Combo( "##var::clicker::i_menu_mode", &i_menu_mode, "Alternar\0Segurar\0\0" ) )
					config.clicker.i_menu_mode = i_menu_mode;
				ImGui::PopItemWidth();

				ImGui::Text( "Validações" );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Condições que impedem a tecla do menu de abrir a interface em momentos indesejados." );

				ImGui::Separator();

				ImGui::Checkbox( "Somente com o Minecraft em foco", &config.clicker.b_menu_only_in_game );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Bloqueia a abertura se outro programa estiver em foco." );

				ImGui::Checkbox( "Bloquear com cursor visível (GUIs, configurações, inventário)", &config.clicker.b_menu_block_gui );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Com o mouse livre na tela existe uma tela do jogo aberta (configurações, inventário, chat) e o menu não abre." );

				ImGui::Checkbox( "Bloquear durante digitação (chat)", &config.clicker.b_menu_block_typing );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Detecta abertura de chat (T ou /) e mantém a tecla do menu bloqueada até o chat fechar (Enter ou Escape)." );

				ImGui::SliderInt( "##var::clicker::i_menu_debounce_ms", &config.clicker.i_menu_debounce_ms, 0, 1000, "Ignorar tecla recente: %d ms" );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Ignora a tecla do menu se qualquer outra tecla foi pressionada nos últimos X ms (protege contra digitação).\n0 desativa." );

				ImGui::Checkbox( "Pausar cliques com o menu aberto", &config.clicker.b_pause_when_menu_open );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Se desativado, o clicker continua rodando com a interface aberta." );

				ImGui::Text( "Estado atual" );
				ImGui::Separator();

				ImGui::Text( "Menu aberto: %s", vars::b_menu_open.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
				ImGui::Text( "Tecla do menu pressionada: %s", vars::b_menu_key_down.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
				ImGui::Text( "Minecraft em foco: %s", vars::key::b_game_focused.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
				ImGui::Text( "Cursor visível (tela/GUI): %s", vars::key::b_cursor_visible.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
				ImGui::Text( "Chat detectado: %s", vars::key::b_chat_open.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
				ImGui::Text( "Capturando tecla: %s", vars::key::b_binding_key.load() ? ICON_FA_CHECK : ICON_FA_TIMES );

				const auto i_last = vars::key::i_last_typing_ms.load();
				const std::string sz_last_typing = i_last ? ( std::to_string( GetTickCount64() - i_last ) + " ms atrás" ) : std::string( "nunca" );
				ImGui::Text( "Última tecla de digitação: %s", sz_last_typing.c_str() );

				ImGui::EndTabItem();
			}

			if ( ImGui::BeginTabItem( "misc" ) )
			{
				ImGui::Text( "Ocultar" );
				ImGui::Separator();
				ImGui::Text( "Tecla para ocultar a janela" );
				keybind_button( config.clicker.i_hide_window_key, 155, 22 );

				ImGui::Text( "Cores" );
				ImGui::Separator();
				ImGui::ColorEdit4( "Cor de destaque", config.clicker.f_color_accent, ImGuiColorEditFlags_NoInputs );
				ImGui::ColorEdit4( "Destaque hover", config.clicker.f_color_accent_hovered, ImGuiColorEditFlags_NoInputs );
				ImGui::ColorEdit4( "Destaque ativo", config.clicker.f_color_accent_active, ImGuiColorEditFlags_NoInputs );
				ImGui::ColorEdit4( "Texto", config.clicker.f_color_accent_text, ImGuiColorEditFlags_NoInputs );

				ImGui::Separator();
				static bool show_advanced_debug { false };
				ImGui::Checkbox( "Mostrar debug avançado", &show_advanced_debug );

				if ( show_advanced_debug )
				{
					ImGui::Text( "Informações" );
					ImGui::Separator();
					ImGui::Text( "Cliques nesta sessão: %d", vars::stats::i_clicks_this_session.load() );
					ImGui::Text( "CPS médio: %.2f", vars::stats::f_average_cps );
					ImGui::Text( "Mestra ativa: %s", vars::key::b_master_active.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
					ImGui::Text( "Clicando à esquerda: %s", vars::stats::b_left_active.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
					ImGui::Text( "Clicando à direita: %s", vars::stats::b_right_active.load() ? ICON_FA_CHECK : ICON_FA_TIMES );
					ImGui::Text( "Botão esquerdo pressionado: %s", vars::key::is_left_down.b_state ? ICON_FA_CHECK : ICON_FA_TIMES );
					ImGui::Text( "Botão direito pressionado: %s", vars::key::is_right_down.b_state ? ICON_FA_CHECK : ICON_FA_TIMES );
					ImGui::Text( "Shift pressionado: %s", vars::key::b_shift_pressed ? ICON_FA_CHECK : ICON_FA_TIMES );
					ImGui::Text( "Inventário aberto: %s", vars::key::b_inventory_opened ? ICON_FA_CHECK : ICON_FA_TIMES );

					if ( !focus::active_window_title().empty() )
						ImGui::Text( "Janela atual: %ls", focus::active_window_title().data() );

					ImGui::Text( "Aplicação: %.1f ms (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				}

				ImGui::Separator();
				ImGui::Text( "https://github.com/b1scoito/clicker" );
				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Clique em mim!" );
				if ( ImGui::IsItemClicked( ImGuiMouseButton_Left ) )
					ShellExecute( 0, 0, L"https://github.com/b1scoito/clicker", 0, 0, SW_SHOW );

				ImGui::EndTabItem();
			}

			if ( ImGui::BeginTabItem( "config" ) )
			{
				ImGui::Text( "Configurações salvas" );
				ImGui::Separator();

				if ( ImGui::Button( "Abrir pasta de configs" ) )
				{
					if ( PIDLIST_ABSOLUTE pidl; SUCCEEDED( SHParseDisplayName( config.get_path().wstring().data(), 0, &pidl, 0, 0 ) ) )
					{
						ITEMIDLIST id_null = { 0 };
						LPCITEMIDLIST pidl_null[1] = { &id_null };
						SHOpenFolderAndSelectItems( pidl, 1, pidl_null, 0 );
						ILFree( pidl );
					}
				}

				constexpr auto& config_items = config.get_configs();
				static auto current_config = -1;

				if ( (size_t) ( current_config ) >= config_items.size() )
					current_config = -1;

				static char buffer[32];

				ImGui::Text( "Configs" );
				if ( ImGui::ListBox( "##var::clicker::config_list", &current_config, []( void* data, int idx, const char** out_text )
				{
					auto& vector = *( std::vector<std::string> * )( data );
					*out_text = vector[idx].c_str();
					return true;
				}, &config_items, (int) ( config_items.size() ), 5 ) && current_config != -1 ) strcpy_s( buffer, config_items[current_config].c_str() );

				if ( ImGui::InputText( "##var::clicker::config_name", buffer, IM_ARRAYSIZE( buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
				{
					if ( current_config != -1 )
						config.rename( current_config, buffer );
				}

				if ( ImGui::Button( "Criar", ImVec2( 60, 25 ) ) )
					config.add( buffer );

				ImGui::SameLine();

				if ( ImGui::Button( "Resetar", ImVec2( 60, 25 ) ) )
					config.reset();

				ImGui::SameLine();

				if ( current_config > -1 )
				{
					if ( ImGui::Button( "Salvar", ImVec2( 60, 25 ) ) )
						config.save( current_config );

					ImGui::SameLine();

					if ( ImGui::Button( "Carregar", ImVec2( 60, 25 ) ) )
						config.load( current_config );

					ImGui::SameLine();

					if ( ImGui::Button( "Excluir", ImVec2( 60, 25 ) ) )
						config.remove( current_config );
				}

				ImGui::EndTabItem();
			}
			
			ImGui::EndTabBar();
		}
	}

	ImGui::End();
}

void c_menu::keybind_button( int& i_key, int i_width, int i_height )
{
	struct bind_state_t
	{
		bool b_capturing = false;
		bool b_held[256] {};
	};

	static std::unordered_map<int*, bind_state_t> states;
	auto& state = states[&i_key];

	const std::string sz_text = state.b_capturing
		? std::string( "Pressione uma tecla" )
		: ( i_key ? ( "Tecla: " + get_key_name_by_id( i_key ) ) : std::string( "Clique para vincular" ) );

	if ( ImGui::Button( sz_text.c_str(), ImVec2( (float) ( i_width ), (float) ( i_height ) ) ) )
	{
		state.b_capturing = !state.b_capturing;

		if ( state.b_capturing )
		{
			for ( auto& [ ptr, other ] : states )
				if ( ptr != &i_key )
					other.b_capturing = false;

			for ( int i = 1; i < 256; ++i )
				state.b_held[i] = ( GetAsyncKeyState( i ) & 0x8000 ) != 0;
		}
	}

	if ( !state.b_capturing )
	{
		// Indicador ao vivo: mostra se a tecla/botão vinculado está pressionado agora
		if ( i_key )
		{
			const bool b_down = ( GetAsyncKeyState( i_key ) & 0x8000 ) != 0;

			ImGui::SameLine();
			ImGui::TextColored(
				b_down ? ImVec4( 0.35f, 0.85f, 0.35f, 1.f ) : ImVec4( 0.85f, 0.35f, 0.35f, 0.85f ),
				b_down ? ICON_FA_CHECK : ICON_FA_TIMES );
		}

		return;
	}

	vars::key::b_binding_key.store( true );

	for ( int i = 1; i < 256; ++i )
	{
		const bool b_down = ( GetAsyncKeyState( i ) & 0x8000 ) != 0;

		if ( b_down && !state.b_held[i] )
		{
			i_key = ( i == VK_ESCAPE ) ? 0 : i;
			state.b_capturing = false;
			break;
		}

		state.b_held[i] = b_down;
	}
}

std::string c_menu::get_key_name_by_id( int id )
{
	static std::unordered_map<int, std::string> key_names = {
		{ 0, "None" },
		{ VK_LBUTTON, "Mouse 1" },
		{ VK_RBUTTON, "Mouse 2" },
		{ VK_MBUTTON, "Mouse 3" },
		{ VK_XBUTTON1, "Mouse 4" },
		{ VK_XBUTTON2, "Mouse 5" },
		{ VK_BACK, "Back" },
		{ VK_TAB, "Tab" },
		{ VK_CLEAR, "Clear" },
		{ VK_RETURN, "Enter" },
		{ VK_SHIFT, "Shift" },
		{ VK_CONTROL, "Ctrl" },
		{ VK_MENU, "Alt" },
		{ VK_PAUSE, "Pause" },
		{ VK_CAPITAL, "Caps Lock" },
		{ VK_ESCAPE, "Escape" },
		{ VK_SPACE, "Space" },
		{ VK_PRIOR, "Page Up" },
		{ VK_NEXT, "Page Down" },
		{ VK_END, "End" },
		{ VK_HOME, "Home" },
		{ VK_LEFT, "Left Key" },
		{ VK_UP, "Up Key" },
		{ VK_RIGHT, "Right Key" },
		{ VK_DOWN, "Down Key" },
		{ VK_SELECT, "Select" },
		{ VK_PRINT, "Print Screen" },
		{ VK_INSERT, "Insert" },
		{ VK_DELETE, "Delete" },
		{ VK_HELP, "Help" },
		{ VK_SLEEP, "Sleep" },
		{ VK_MULTIPLY, "*" },
		{ VK_ADD, "+" },
		{ VK_SUBTRACT, "-" },
		{ VK_DECIMAL, "." },
		{ VK_DIVIDE, "/" },
		{ VK_NUMLOCK, "Num Lock" },
		{ VK_SCROLL, "Scroll" },
		{ VK_LSHIFT, "Left Shift" },
		{ VK_RSHIFT, "Right Shift" },
		{ VK_LCONTROL, "Left Ctrl" },
		{ VK_RCONTROL, "Right Ctrl" },
		{ VK_LMENU, "Left Alt" },
		{ VK_RMENU, "Right Alt" },
	};

	if ( id >= 0x30 && id <= 0x5A )
		return std::string( 1, (char) id );

	if ( id >= 0x60 && id <= 0x69 )
		return "Num " + std::to_string( id - 0x60 );

	if ( id >= 0x70 && id <= 0x87 )
		return "F" + std::to_string( ( id - 0x70 ) + 1 );

	return key_names[id];
}

void c_menu::set_position( int x, int y, int cx, int cy, HWND hwnd )
{
	POINT point; GetCursorPos( &point );

	auto flags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE;
	if ( x != 0 && y != 0 )
	{
		x = point.x - x;
		y = point.y - y;
		flags &= ~SWP_NOMOVE;
	}

	if ( cx != 0 && cy != 0 )
		flags &= ~SWP_NOSIZE;

	SetWindowPos( hwnd, nullptr, x, y, cx, cy, flags );
}

void c_menu::get_mouse_offset( int& x, int& y, HWND hwnd )
{
	POINT point; RECT rect;

	GetCursorPos( &point );
	GetWindowRect( hwnd, &rect );

	x = point.x - rect.left;
	y = point.y - rect.top;
}

// Taken from: https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx9/main.cpp
LRESULT WINAPI c_menu::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return true;

	switch ( msg )
	{
		case WM_SIZE:
			if ( directx9::device != NULL && wParam != SIZE_MINIMIZED )
			{
				directx9::param.BackBufferWidth = LOWORD( lParam );
				directx9::param.BackBufferHeight = HIWORD( lParam );
				ResetDevice();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

bool c_menu::CreateDeviceD3D( HWND hWnd )
{
	if ((directx9::context = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&directx9::param, sizeof(directx9::param));
	{
		directx9::param.Windowed = TRUE;
		directx9::param.SwapEffect = D3DSWAPEFFECT_DISCARD;
		directx9::param.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
		directx9::param.EnableAutoDepthStencil = TRUE;
		directx9::param.AutoDepthStencilFormat = D3DFMT_D16;
		directx9::param.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
	}

	if (directx9::context->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &directx9::param, &directx9::device) < 0)
		return false;

	return true;
}

void c_menu::CleanupDeviceD3D()
{
	if (directx9::device) { directx9::device->Release(); directx9::device = NULL; }
	if (directx9::context) { directx9::context->Release(); directx9::context = NULL; }
}

void c_menu::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = directx9::device->Reset(&directx9::param);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void c_menu::destroy()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(ctx::hWnd);
	UnregisterClass(ctx::wc.lpszClassName, ctx::wc.hInstance);
}

void c_menu::render()
{
	const auto clear_color = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);

	auto done = false;
	while (!done)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				done = true;
		}

		if (done)
			break;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto* colors = ctx::imgui_style->Colors;
		colors[ImGuiCol_Text] = float_to_imvec4(config.clicker.f_color_accent_text);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
		colors[ImGuiCol_Border] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.54f);
		colors[ImGuiCol_TitleBgActive] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
		colors[ImGuiCol_ScrollbarGrab] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_TextSelectedBg] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_CheckMark] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_SliderGrab] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_SliderGrabActive] = float_to_imvec4(config.clicker.f_color_accent_active);
		colors[ImGuiCol_Button] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_ButtonHovered] = float_to_imvec4(config.clicker.f_color_accent_hovered);
		colors[ImGuiCol_ButtonActive] = float_to_imvec4(config.clicker.f_color_accent_active);
		colors[ImGuiCol_Header] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_HeaderHovered] = float_to_imvec4(config.clicker.f_color_accent_hovered);
		colors[ImGuiCol_HeaderActive] = float_to_imvec4(config.clicker.f_color_accent_active);
		colors[ImGuiCol_Separator] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_Tab] = float_to_imvec4(config.clicker.f_color_accent);
		colors[ImGuiCol_TabHovered] = float_to_imvec4(config.clicker.f_color_accent_hovered);
		colors[ImGuiCol_TabActive] = float_to_imvec4(config.clicker.f_color_accent_active);

		on_paint();

		ImGui::EndFrame();

		directx9::device->SetRenderState(D3DRS_ZENABLE, FALSE);
		directx9::device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		directx9::device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		directx9::device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

		if (directx9::device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			directx9::device->EndScene();
		}

		HRESULT result = directx9::device->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && directx9::device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();

	}
	
	destroy();
}

bool c_menu::setup()
{
	ImGui_ImplWin32_EnableDpiAwareness();
	ctx::wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, L"Class", NULL };
	RegisterClassEx( &ctx::wc );

	ctx::hWnd = CreateWindow(ctx::wc.lpszClassName, L"", WS_POPUP, 100, 100, 600, 350, NULL, NULL, ctx::wc.hInstance, NULL);
	if ( !CreateDeviceD3D(ctx::hWnd) )
	{
		CleanupDeviceD3D();
		UnregisterClass( ctx::wc.lpszClassName, ctx::wc.hInstance );

		return false;
	}

	ShowWindow(ctx::hWnd, SW_SHOWDEFAULT );
	UpdateWindow(ctx::hWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ctx::imgui_io = &ImGui::GetIO();
	ctx::imgui_style = &ImGui::GetStyle();

	ctx::imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if (PWSTR fonts_path; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &fonts_path)))
	{
		const std::filesystem::path path { fonts_path };
		CoTaskMemFree(fonts_path);

		ctx::imgui_io->Fonts->AddFontFromFileTTF((path / "SegoeUI.ttf").string().data(), 16.f, NULL, ctx::imgui_io->Fonts->GetGlyphRangesDefault());
	}

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

	ctx::imgui_io->Fonts->AddFontFromMemoryCompressedTTF( fontawesome_compressed_data, fontawesome_compressed_size, 10.f, &icons_config, icons_ranges );

	ctx::imgui_style->ScrollbarSize = 10.0f;
	ctx::imgui_style->GrabRounding = 5.0f;
	ctx::imgui_style->GrabMinSize = 10.0f;
	ctx::imgui_style->FrameRounding = 3.0f;
	ctx::imgui_style->TabRounding = 3.0f;

	ctx::imgui_io->IniFilename = nullptr;

	ImGui_ImplWin32_Init(ctx::hWnd);
	ImGui_ImplDX9_Init( directx9::device );

	return true;
}
