// swdeck32.h

// deck management functions for the Win32 SWCCG DeckMaker


/*

Notes on new formats:

masterlist[] can't assume now that the list is either all dark
or all light, since both light and dark will be loaded.

Make one data file (swccgdm.dat) that contains all the cards,
in the same order as in 2.01.  When a 2.01 deck is loaded,
adjust the masterlist[] indexes to the appropriate side. (Light
will come after Dark.)

Masterlist entries will disappear.  Which is no big deal since
the idx will be kept in the cardlist[] array.  Just make sure
to make masterlist[].bValid=FALSE so it'll be sorted out in
the newsort() ranking.

Instead of masterlist[] containing an array of pointers into a
hCardData memory block, have it contain actual pointers into
the actual memory.  First, load the entire swccgdm.dat file and
then assign pointers by running through the long string.

*/

#define MAXLIST		1200

#define LIGHT		1
#define DARK		0

// suggested maximum size of temporary buffers
#define MAXNAME		70
#define MAXATT		16
#define MAXDESC		350

int		szCardData;
int		iDeckSide;

BOOL	bValidData;

char	*CardData;	// OpenSWCCGData will load all of swccgdm.dat
					// into this, then fill out masterlist[] with
					// the correct pointers into it

struct mastercard
{
	BOOL	bValid;
	int		iSide;			// this card is either LIGHT or DARK
	char	*name;			// these pointers will be assigned
	char	*attributes;	// into one big dynamically-allocated
	char	*description;	// string (char *CardData)
	int		rank;
}

struct mastercard masterlist[MAXLIST];

struct card
{
	int		idx;
	int		inventory;
	int		deck;
}

struct card cardlist[MAXLIST];

// to handle sets, a small file called swccgdm.set will list
// each set and their code.  The first character in each line
// will be the code, the rest is the set's name.
int		iNumberOfSets;	// number of sets loaded in swccgdm.set
char	sets[MAXSETS][16];

