#include "sys/alt_stdio.h"
#include "system.h"
#include <unistd.h>
#include "sys/alt_irq.h"

unsigned char seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
char	hex_segments[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

volatile int * BOTTONS_ptr = (int *) BOTTONS_BASE;

volatile int edge_capture;
volatile int * Timer_POINTER = (int *) TIMER_BASE;

short seconds = 0 ;
short minutes = 0 ;
short heurs   = 0 ;

int affiche_Mode 	= 	0x1 ;                // 1 : mode normale   / 0 : mode chrono
int chrono_etat  	=  	0x0 ;    			// 1 : etat declanche / 0 : etat pause
int change_temps 	=  	0x0 ; 		        // 1 : mode change    / 0 : mode sans chang. par l'utilisateur
int change_droite 	= 	0x1 ; 		         // 1 : change min  / 0 : change heurs

short temps_minute ;
short temps_heurs ;
short temps_seconds ;

short chrono_minute ;
short chrono_heurs ;
short chrono_seconds ;

void affiche_hex_min_sec();
void affiche_hex_heur_min();
short separer_num_2_time(short num);


void Update_HEX_display( int buffer )
{
	volatile int * HEX3_HEX0_ptr = (int *) HEX_BASE;
	int shift_buffer, nibble;
	char code;
	int i;

	shift_buffer = buffer;
	for ( i = 0; i < 4; ++i )
	{
		nibble = shift_buffer & 0x0000000F;		// character is in rightmost nibble
		code = seven_seg_decode_table[nibble];
		hex_segments[i] = code;
		shift_buffer = shift_buffer >> 4;
	}
	*(HEX3_HEX0_ptr) = *(int *) hex_segments; 		// drive the hex displays
	return;
}

void Increment_time_chrono ( )
{
		chrono_seconds ++ ;
		if (chrono_seconds > 60)
		{
			chrono_seconds = 0 ;
			chrono_minute ++  ;
			if (chrono_minute > 60 )
				{
				chrono_minute = 0 ;
				chrono_heurs ++ ;

				if ( chrono_heurs > 24) chrono_heurs = 0 ;
			}
		}
}

void Increment_time_temps ()
{
		temps_seconds ++ ;
		if (temps_seconds > 60)
		{
			temps_seconds = 0 ;

			temps_minute  ++  ;

			if (temps_minute > 60 )
				{
				temps_minute = 0 ;
				temps_heurs ++ ;
				if (temps_heurs > 24) temps_heurs = 0 ;
			}
		}
}


void init_timer(){
	void * edge_capture_ptr = (void*) & edge_capture ;
	
	temps_heurs = 0 ; temps_minute = 0 ; temps_seconds = 0 ;
	chrono_heurs = 0 ; chrono_minute = 0 ;	chrono_seconds = 0 ;

	*(Timer_POINTER + 2) = 0XF ; 				// Masque de l’interruption
	alt_irq_register (TIMER_IRQ, edge_capture_ptr,handle_TIMER_interrupts);
}

void handle_TIMER_interrupts (void* context, alt_u32 id)
{
	int rised_timer = * (Timer_POINTER +3); // lecture du registre du periph

	if ( rised_timer & 0X1)
	{
		if ( !change_temps )
			Increment_time_temps();

		if ( (!afficheMode) && chrono_etat )
		{
			Increment_time_temps();
			Increment_time_chrono();
		}
	}

	( (afficheMode & (!change_temps)) == 1 ) ? affiche_hex_heur_min(temps) : affiche_hex_min_sec(chrono);

	*(Timer_POINTER + 3) = 0 ; // effacer le registre du periph
}


void affiche_hex_heur_min()
{
	int buff_heurs, buff_minute;
	int Buffer_HEX ;

	buff_heurs =separer_num_2_time  (temps_heurs);
	buff_minute =separer_num_2_time  (temps_minute);

	Buffer_HEX = buff_minute + (buff_heurs << 8);
	Update_HEX_display(Buffer_HEX);
}


void affiche_hex_min_sec()
{
	int buff_minute, buff_seconds;
	int Buffer_HEX ;

	buff_minute    =  separer_num_2_time(chrono_minute);
	buff_seconds   =  separer_num_2_time(chrono_seconds);

	Buffer_HEX = buff_seconds + (buff_minute << 8);
	Update_HEX_display(Buffer_HEX);
}


short separer_num_2_time(short num)
{
	short num_dec, num_unit ;
	for (int i = 0 ; i<= 5 ; i++)
	{
		if((int)( num / (i*10)) == 1 )
		{
			num_dec = i;
			break;
		}
	}
	num_unit = num - (num_dec*10) ;

	return (num_unit + (num_dec << 4)) ;
}


void handle_KEY_interrupts(void* context, alt_u32 id)
{
	int press = * (KEY_POINTER + 3 ); // lecture du registre du periph

	if ( press & 0X1) 		// KEY 0 RESET      / exite mode change temps
	{
		if ( change_temps | (!afficheMode) )
		{
			temps_heurs = 0 ; temps_minute = 0 ; temps_seconds = 0 ;
			chrono_minute = 0 ;   chrono_minute = 0 ;  chrono_minute = 0 ;
		}
	}

	else if ( press & 0X4 ) // KEY 2 PAUSE      / --
	{
		if ( change_temps )
		{
			// decrementation du valeur
			change_droite ? dec_temps_conf( temps_minute , 60) : dec_temps_conf( temps_heurs , 23);
		}
		else if ( !change_temps )
			chrono_etat = 0 ;
	}

	else if ( press & 0X2 ) // KEY 1 DECLANCHER / ++
	{
		if ( change_temps )
		{
			// incrementation de la valeur
			(change_droite == 1 ) ? incr_temps_conf(&(temps_minute) , 60): incr_temps_conf( & (temps_heurs) , 23);
		}
		else if ( !change_temps )
			chrono_etat = 1  ;
	}

	else if ( press & 0X8 ) // KEY 3 / mode normale => mode chrono || mode => mode
	{
		if ( afficheMode ) 		// chang. mode normale => mode chrono

			afficheMode = 0 ;


		else if ( (!afficheMode) & (!change_temps) ) 	// chang. mode chrono => mode change_temps
		{
			change_temps  = 1 ;
			change_droite = 0 ;
		}
		else if ( (!afficheMode) & change_temps ) 	  // chang. mode change_temps => mode normale
		{
			change_droite = 1  ;
		}
		else if ( (!afficheMode) & change_temps  & change_droite ) // chang. mode change_temps => mode normal
		{
			change_temps  = 0 ;
			change_droite = 0 ;
			afficheMode   = 1  ;
		}
	}
	(change_temps == 1 ) ?  *(Timer_POINTER + 3) = 0 : *(Timer_POINTER + 3) = 0xf ;
	if ( change_temps )
	affiche_hex_heur_min(temps);

	*(KEY_POINTER + 3) = 0 ; // effacer le registre du periph
}

void init_key()
{
	void * edge_capture_ptr = (void*) & edge_capture ;

	*(KEY_POINTER + 2) = 0XF ; // Masque de l’interruption
	alt_irq_register ( KEY_IRQ, edge_capture_ptr,handle_KEY_interrupts);
}



void incr_temps_conf (short &var_temps, short max_var)
{
	(*var_temps < max_var) ? *var_temps ++ : *var_temps = 0 ;
}

void decr_temps_conf(short var_temps, short max_var)
{
	(*var_temps > 0  ) ? *var_temps -- : *var_temps = max_var ;
}

int main()
{
	init_key();
	init_timer();

	return 0 ;

}

