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

		C	= character
		CR	= rebel
		CI	= imperial
		CA	= alien
		CD	= droid

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

3		Deploy/Interior or Exterior site(I or E)
4		Forfeit
5		Power/Ferocity/Rebel Force
6		Ability/Armor/Maneuver/Defense/Imperial Force
7		Pilot(Y/N)/Hyperdrive/Landspeed/Parsec
8		Warrior(Y/N)/Perm. Pilot(Y/N)
9		Rarity		R  U  C   N
10		Subrarity	12 12 123 A
11		Set			L(Premiere) P(2P) A(ANH) R(Rebel Leader)
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

    N	Interior
	X	Exterior
	D	Dejarik

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


// these are indexes into the "icon" array aPics[].
#define ICO_NONE	0
#define ICO_BLANK	1
#define ICO_DEST0	2
#define ICO_DEST1   3
#define ICO_DEST2	4
#define ICO_DEST3	5
#define ICO_DEST4	6
#define ICO_DEST5	7
#define ICO_DEST6	8
#define ICO_DEST7   9
#define ICO_SYSTEM	10
#define ICO_INTSITE	11
#define ICO_EXTSITE	12
#define ICO_MOBILE	13
#define ICO_DEJARIK	14
#define ICO_REBEL	15
#define ICO_IMP		16
#define ICO_DROID	17
#define ICO_ALIEN	18
#define ICO_CRITTER	19
#define ICO_VEHICLE	20
#define ICO_SHIP	21
#define ICO_INT		22
#define ICO_UORL	23
#define ICO_EFFECT	24
#define ICO_UTINNI	25
#define ICO_IMMED	26
#define ICO_RFORCE0	27
#define ICO_RFORCE1	28
#define ICO_RFORCE2	29
#define ICO_RFORCE3	30
#define ICO_IFORCE0	31
#define ICO_IFORCE1	32
#define ICO_IFORCE2	33
#define ICO_IFORCE3	34
#define ICO_WEAPON	35
#define ICO_DEVICE	36
#define ICO_PILOT	37
#define ICO_ASTRO	38
#define ICO_WARRIOR	39
#define ICO_R1		40
#define ICO_R2		41
#define ICO_U1		42
#define ICO_U2		43
#define ICO_C1		44
#define ICO_C2		45
#define ICO_C3		46
#define ICO_NA		47
#define ICO_SCOMP	48
#define ICO_LTD		49
#define ICO_ULTD	50
#define ICO_2P		51
#define ICO_ANH		52
#define ICO_RL		53
#define ICO_EVENT	54
#define ICO_LOST	55
#define ICO_HOTH	56
#define ICO_DESTPI	57
#define ICO_DESTX   58
#define ICO_DEPLOY0 59
#define ICO_DEPLOY1 60
#define ICO_DEPLOY2 61
#define ICO_DEPLOY3 62
#define ICO_DEPLOY4 63
#define ICO_DEPLOY5 64
#define ICO_DEPLOY6 65
#define ICO_DEPLOY7 66
#define ICO_DEPLOY8 67
#define ICO_DEPLOY9 68
#define ICO_DEPLOYX 69
#define ICO_FORFEIT0 70
#define ICO_FORFEIT1 71
#define ICO_FORFEIT2 72
#define ICO_FORFEIT3 73
#define ICO_FORFEIT4 74
#define ICO_FORFEIT5 75
#define ICO_FORFEIT6 76
#define ICO_FORFEIT7 77
#define ICO_FORFEIT8 78
#define ICO_FORFEIT9 79
#define ICO_FORFEITX 80





#define CTYPE		0
#define CDEST		1
#define CRARITY		2
#define CSET		3
#define CEXTRA1		4
#define CEXTRA2		5
#define CEXTRA3		6

#define RGB_BLUE		RGB(0,0,175)		// for ships and vehicles
#define RGB_RED			RGB(175,0,0)		// for effect and interrupts
#define RGB_GREEN		RGB(0,120,0)		// for weapons and devices
#define RGB_BLACK		RGB(0,0,0)			// for all characters
#define RGB_GREY		RGB(120,64,64) 		// for systems and sites
#define RGB_WHITE		RGB(255,255,255)
#define RGB_PURPLE		RGB(128,0,128)		// for epic events
#define RGB_DKGREY		RGB(96,96,96)		// for creatures
