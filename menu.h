#pragma once
#include "main.h"
#include <functional>

struct Vars_s
{
	bool esp_box = false;
	bool esp_health = false;
	bool backtrack = false;
	int backtrack_ticks = 4;
	bool backtrack_draw = false;
	int backtrack_draw_r = 255;
	int backtrack_draw_g = 255;
	int backtrack_draw_b = 255;
	bool backtrack_visible = false;
	bool antiscreen = false;
	bool bunnyhop = false;
};

extern Vars_s* vars;

namespace menu
{
	static bool keystate[ 255 ] = {};
	static bool oldkeystate[ 255 ] = {};

	extern unsigned long font;
	extern unsigned long smallfont;
	extern void Rect( int x0, int y0, int x1, int y1, int r, int g, int b, int a );
	extern void FilledRect( int x0, int y0, int x1, int y1, int r, int g, int b, int a );
	extern void Text( int x, int y, int r, int g, int b, int a, int alignment, bool lowersize, const char* msg, ... );
	extern bool IsKeyDown( int key );
	extern bool WasKeyPressed( int key );

	class Element
	{
	public:
		static std::vector<Element*> elements;
		static int input_currentelement;

		inline void SetName( const char* n )
		{
			element = n;
		}

		inline const char* GetName( )
		{
			return element;
		}

		inline void SetPos( int _x, int _y )
		{
			x = _x;
			y = _y;
		}

		inline void Add( Element* el )
		{
			SetPos( el->x, el->y + ( elements.size( ) * 23 ) );
			el->elements.push_back( this );
		}

		virtual void Draw( ) {};
		virtual void Think( ) {};

		int x;
		int y;
	private:
		const char* element;
	};

	// input handling
	class Input : public Element
	{
	public:
		Input( )
		{
			// don't need to add this as element, but we will get how much elements are registered
			elementcount = elements.size( ) - 1;

			// don't need to set position for this
		}

		void Think( )
		{
			std::copy( keystate, keystate + 255, oldkeystate );

			for( int i = 0; i < 255; i++ )
				keystate[ i ] = GetAsyncKeyState( i );

			if( !hold )
			{
				if( IsKeyDown( VK_UP ) )
				{
					currentelement--;
					hold = true;
				}
				else if( IsKeyDown( VK_DOWN ) )
				{
					currentelement++;
					hold = true;
				}

				if( currentelement > elementcount ) currentelement = 0;
				else if( currentelement < 0 ) currentelement = elementcount;

				input_currentelement = currentelement;
			}
			else
			{
				if( !IsKeyDown( VK_UP ) && !IsKeyDown( VK_DOWN ) )
					hold = false;
			}
		}
	private:
		int elementcount = 0;
		int currentelement = 0;
		bool hold = false;
	};

	class Checkbox : public Element
	{
	public:
		Checkbox( )
		{
			thiselement = elements.size( );
		}

		inline void SetToggle( bool* t )
		{
			toggle = t;
		}

		void Draw( )
		{
			static int w = 220;
			static int h = 23;
			static int alpha = 210;

			// outline
			menu::Rect( x - 1, y - 1, ( x + w ) + 1, ( y + h ) + 1, 0, 0, 0, 255 );

			menu::FilledRect( x, y, x + w, y + h, 75, 75, 200, alpha );
			if( !*toggle )
				menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 122, 122, 122, alpha + 25 );
			else
				menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 24, 200, 100, alpha + 11 );

			if( input_currentelement == thiselement )
				menu::FilledRect( x, y, x + w, y + h, 82, 82, 82, alpha - 80 );

			// text
			if( input_currentelement == thiselement )
				menu::Text( x + 5, y + 10, 219, 235, 0, 255, 0, false, GetName( ) );
			else
				menu::Text( x + 5, y + 10, 255, 255, 255, 255, 0, false, GetName( ) );

			if( *toggle )
				menu::Text( x + w - 28, y + 10, 255, 255, 255, 255, 1, false, "on" );
			else
				menu::Text( x + w - 28, y + 10, 255, 255, 255, 255, 1, false, "off" );
		}

		void Think( )
		{
			if( input_currentelement == thiselement )
			{
				if( !hold )
				{
					if( IsKeyDown( VK_LEFT ) || IsKeyDown( VK_RIGHT ) )
					{
						*toggle = !*toggle;
						hold = true;
					}
				}
				else
				{
					if( !IsKeyDown( VK_LEFT ) && !IsKeyDown( VK_RIGHT ) )
						hold = false;
				}
			}
		}
	private:
		int thiselement;
		bool* toggle;
		bool hold = false;
	};

	class Slider : public Element
	{
	public:
		Slider( )
		{
			thiselement = elements.size( );
			add = 1;
		}

		inline void SetVariable( int* v )
		{
			var = v;
		}

		inline void SetMinMax( int __min, int __max )
		{
			_min = __min;
			_max = __max;
		}

		inline void SetAdd( int _add )
		{
			add = _add;
		}

		void Draw( )
		{
			static int w = 220;
			static int h = 23;
			static int alpha = 210;

			// outline
			menu::Rect( x - 1, y - 1, ( x + w ) + 1, ( y + h ) + 1, 0, 0, 0, 255 );

			menu::FilledRect( x, y, x + w, y + h, 75, 75, 200, alpha );
			menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 24, 200, 100, alpha + 5 );

			if( input_currentelement == thiselement )
				menu::FilledRect( x, y, x + w, y + h, 82, 82, 82, alpha - 80 );

			// text
			if( input_currentelement == thiselement )
				menu::Text( x + 5, y + 10, 219, 235, 0, 255, 0, false, GetName( ) );
			else
				menu::Text( x + 5, y + 10, 255, 255, 255, 255, 0, false, GetName( ) );

			menu::Text( x + w - 28, y + 10, 255, 255, 255, 255, 1, false, "%i", *var );
		}

		void Think( )
		{
			if( input_currentelement == thiselement )
			{
				if( !hold )
				{
					// *var++ and *var-- don't work for some reason
					if( IsKeyDown( VK_LEFT ) )
					{
						*var = *var - add;
						hold = true;
					}
					else if( IsKeyDown( VK_RIGHT ) )
					{
						*var = *var + add;
						hold = true;
					}

					if( *var > _max ) * var = _min;
					else if( *var < _min ) * var = _max;
				}
				else
				{
					if( !IsKeyDown( VK_LEFT ) && !IsKeyDown( VK_RIGHT ) )
						hold = false;
				}
			}
		}
	private:
		int thiselement;
		int* var;
		int _min = 0;
		int _max = 0;
		int add = 0;
		bool hold = false;
	};

	class Key : public Element
	{
	public:
		Key( )
		{
			thiselement = elements.size( );
		}

		inline void SetVariable( int* v )
		{
			var = v;
		}

		inline const char* ConvertKey( int vkey )
		{
			static char ret[ 32 ];

			auto map = MapVirtualKeyW( vkey, MAPVK_VK_TO_CHAR );

			if( map )
			{
				if( map == 32 ) return "Space";
				else if( map == '\t' ) return "Tab";
				else if( map == '\b' ) return  "Backspace";
				else if( map == '\n' || map == '\r' ) return "Enter";
				else
				{
					sprintf_s( ret, "%c", map );
					return ret;
				}
			}
			else
			{
				switch( vkey )
				{
					case VK_LBUTTON: return "LM";
					case VK_RBUTTON: return "RM";
					case VK_MBUTTON: return "MM";
					case VK_INSERT: return "Insert";
					case VK_DELETE: return "Delete";
					case VK_SHIFT: return "LSHIFT";
					case VK_MENU: return "Alt";
					case VK_CONTROL: return "Control";
					case VK_XBUTTON1: return "XB1";
					case VK_XBUTTON2: return "XB2";
					case VK_HOME: return "Home";
					case VK_PRIOR: return "Page up";
					case VK_NEXT: return "Page down";
					case VK_CAPITAL: return "Caps lock";

				}
			}

			return "[unknown]";
		}

		inline void SetKey( int vkey )
		{
			chkey = ConvertKey( vkey );
			if( *var ) // set var only if it's been not nullptr
				* var = vkey;
		}

		void Draw( )
		{
			static int w = 220;
			static int h = 23;
			static int alpha = 210;

			// outline
			menu::Rect( x - 1, y - 1, ( x + w ) + 1, ( y + h ) + 1, 0, 0, 0, 255 );

			menu::FilledRect( x, y, x + w, y + h, 75, 75, 200, alpha );
			menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 24, 200, 100, alpha + 5 );

			if( input_currentelement == thiselement )
				menu::FilledRect( x, y, x + w, y + h, 82, 82, 82, alpha - 80 );

			// text
			if( input_currentelement == thiselement )
				menu::Text( x + 5, y + 10, 219, 235, 0, 255, 0, false, GetName( ) );
			else
				menu::Text( x + 5, y + 10, 255, 255, 255, 255, 0, false, GetName( ) );

			if( gettingkey )
				menu::Text( x + w - 28, y + 10, 255, 255, 255, 255, 1, false, "..." );
			else
				menu::Text( x + w - 28, y + 10, 255, 255, 255, 255, 1, true, chkey );
		}

		void Think( )
		{
			if( input_currentelement == thiselement )
			{
				if( !hold )
				{
					if( IsKeyDown( VK_LEFT ) || IsKeyDown( VK_RIGHT ) )
					{
						gettingkey = true;
						hold = true;
					}
				}
				else
				{
					if( !IsKeyDown( VK_LEFT ) && !IsKeyDown( VK_RIGHT ) )
						hold = false;
				}

				if( gettingkey )
				{
					static HWND valvewindow = 0;
					while( !valvewindow )
						valvewindow = FindWindowA( "Valve001", 0 );

					if( GetActiveWindow( ) != valvewindow ) return;

					for( int i = 0; i < 5; i++ )
					{
						if( !gettingkey ) break;

						if( IsKeyDown( i ) )
						{
							switch( i )
							{
								case 0:
									chkey = "LM";
									*var = i;
									gettingkey = false;
									break;
								case 1:
									chkey = "RM";
									*var = i;
									gettingkey = false;
									break;
								case 2:
									chkey = "MM";
									*var = i;
									gettingkey = false;
									break;
								case 3:
									chkey = "XB1";
									*var = i;
									gettingkey = false;
									break;
								case 4:
									chkey = "XB2";
									*var = i;
									gettingkey = false;
									break;
							}
						}
					}

					for( int i = VK_BACK; i <= VK_RMENU; i++ )
					{
						if( !gettingkey ) break;

						if( ( WasKeyPressed( i ) ) && ( i != VK_LEFT ) && ( i != VK_RIGHT ) )
						{
							if( i == VK_ESCAPE )
							{
								chkey = "[unknown]";
								*var = 0;
								gettingkey = false;
								break;
							}

							chkey = ConvertKey( i );
							*var = i;
							gettingkey = false;
							break;
						}
					}
				}
			}
		}
	private:
		int thiselement;
		int* var;
		bool hold = false;
		bool gettingkey = false;
		const char* chkey = "";
	};

	class Button : public Element
	{
	public:
		Button( )
		{
			thiselement = elements.size( );
		}

		inline void SetFunction( std::function<void( )> f )
		{
			function = f;
		}

		void Draw( )
		{
			static int w = 220;
			static int h = 23;
			static int alpha = 210;

			// outline
			menu::Rect( x - 1, y - 1, ( x + w ) + 1, ( y + h ) + 1, 0, 0, 0, 255 );

			menu::FilledRect( x, y, x + w, y + h, 75, 75, 200, alpha );
			menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 24, 200, 100, alpha + 5 );

			if( input_currentelement == thiselement )
				menu::FilledRect( x, y, x + w, y + h, 82, 82, 82, alpha - 80 );

			// text
			if( input_currentelement == thiselement )
				menu::Text( x + 5, y + 10, 219, 235, 0, 255, 0, false, GetName( ) );
			else
				menu::Text( x + 5, y + 10, 255, 255, 255, 255, 0, false, GetName( ) );

			if( !hold )
				menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 122, 122, 122, alpha + 25 );
			else
				menu::FilledRect( x + w - 55, y + 1, x + w - 1, y + h - 1, 24, 200, 100, alpha + 11 );
		}

		void Think( )
		{
			if( input_currentelement == thiselement )
			{
				if( !hold )
				{
					if( IsKeyDown( VK_LEFT ) || IsKeyDown( VK_RIGHT ) )
					{
						function( );
						hold = true;
					}
				}
				else
				{
					if( !IsKeyDown( VK_LEFT ) && !IsKeyDown( VK_RIGHT ) )
						hold = false;
				}
			}
		}
	private:
		int thiselement;
		bool hold = false;
		std::function<void( )> function;
	};

	extern Element* body;
	extern Input* input;

	extern void AddCheckbox( const char* name, bool* toggle );
	extern void AddSlider( const char* name, int* variable, int min, int max, int add = 1 );
	extern void AddKey( const char* name, int* variable, int defaultkey );
	extern void AddButton( const char* name, std::function<void( )> func );

	extern void Init( );
	extern void Draw( );
}