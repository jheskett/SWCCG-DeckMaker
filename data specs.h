/*

SWCCGDM.DAT Format

Attributes\0Name\0Game_Text\0

Attributes (15 bytes)

Byte	Use
0		Destiny

1		Type
2		Subtype

		L	= location
		LP	= system
		LT	= site
		LM	= mobile system
		LV	= mobile site
		LS	= sector

		C	= character
		CR	= rebel
		CI	= imperial
		CA	= alien
		CD	= droid
		CJ	= jedi master

		R	= creature

		V	= vehicle
		VT	= transport
		VC	= creature
		VS	= shuttle

		S	= starship
		SC	= capital
		SF	= starfighter
		SS	= squadron

		I	= interrupt
		IL	= lost
		IU	= used
		IB	= used or lost
		IS	= starting

		E	= effect
		EE	= normal
		EU	= utinni
		EI	= immediate

		W	= weapon
		WD	= death star
		WC	= character
		WA	= automated
		WS	= starship

		D	= device

		N	= event

		J	= jedi test

3		Deploy/Interior or Exterior site(I or E)
4		Forfeit
5		Power/Ferocity/Rebel Force
6		Ability/Armor/Maneuver/Defense/Imperial Force
7		Pilot(Y/N)/Hyperdrive/Landspeed/Parsec
8		Warrior(Y/N)/Perm. Pilot(Y/N)
9		Rarity		R   U   C    N
10		Subrarity	012 012 0123 A
11		Set
			L(Premiere) P(2P) A(ANH) R(Rebel Leader)
12		Side L or D (light or dark)
13		Astromech(Y/N)
14		extra icon

extra icon

	I	Imperial
	R	Rebel
	S	Scomp Link
	D	Dejarik
	1-6	Destiny
	W	Warrior
	A	Astromech (non-ships)


Icons to make:

Interior
Exterior
Mobile System
Scomp Link
Dejarik

*/

#define ATT_DESTINY	  0
#define ATT_TYPE	  1
#define ATT_SUBTYPE   2	
#define	ATT_DEPLOY	  3
#define ATT_EXTINT	  3
#define ATT_FORFEIT	  4
#define ATT_POWER	  5
#define ATT_RFORCE	  5
#define ATT_ABILITY	  6
#define ATT_DEFENSE	  6
#define ATT_IFORCE	  6
#define ATT_PILOT	  7
#define ATT_SPEED	  7
#define	ATT_PARSEC	  7
#define ATT_WARRIOR	  8
#define ATT_PPILOT	  8
#define	ATT_RARITY	  9
#define ATT_SUBRARITY 10
#define	ATT_SET		  11
#define ATT_SIDE	  12
#define ATT_ASTRO	  13
#define ATT_ICON	  14
