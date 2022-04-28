// help.h

#define HELP_EXCLUDE "To make the list more manageable, one or several \
groups of cards can be excluded from the list.  (ie, if \
you're making a Light deck, you're usually not concerned \
with the Dark cards; or, if you're not interested in whether \
a card is Unlimited or Limited, you'll probably want to keep the \
Unlimited set excluded.)  These groups can be Included back at \
any time.  By checking 'Keep used cards', only cards \
with zero quantity will be excluded.  The rest will remain in the \
list.  If 'Keep used cards' is unchecked, then ALL cards of the selected group(s) \
will be removed from the list, regardless of quantity. (When the removed \
cards are Included later, they will retain their quantity)"

#define HELP_INCLUDE "If you have Excluded cards from view, you can \
Include them back.  (ie, if you've Excluded Unlimited and you want to \
see Unlimiteds in the list again, you can Include them back)  Note that \
when the cards are Included back they retain their previous quantity."

#define HELP_NOSETS "The file 'SWCCG Sets.dm' contains the codes used for\r\
each set.  The program can't continue without them.\r\
Would you like to create the default Sets file?"

#define HELP_SORT "Through the Custom Sort, you have some limited \
control over the order the cards are listed.  The first sort you pick \
will be the primary sort, the second will be the secondary sort, etc. \
You can't change the order within a sort.  (ie, Rares will always list \
before Uncommons in a Rarity sort)  If a sort you define comes out nothing \
like you expected, check to make sure that you have narrowed it down \
enough.  For instance, if you wanted to sort all cards by type and \
the subtypes(ie, rebels, aliens, droids separate), 'Type' must list \
before 'Subtype'.  Another example: If you pick 'Deploy' as your only sort, \
then all non-deploy cards(interrupts, effects, etc) will list before \
deployable cards, possibly creating undesirable results.  In this case, you \
should have a 'Type' sort occur before the 'Deploy' sort.  Once all cards \
are sorted by the list you provide, the cards are sorted by Name last.  If \
you want to sort the cards by Name only, you can leave the list empty. \
The menu item 'by Custom Sort' must be checked \
in order to view the sort.  This is automatically selected when you define \
a new custom sort.  If you then go to another sort(ie, 'by Name') you can \
just hit 'by Custom Sort' to return to the defined sort.  The current Custom \
Sort will be remembered even between sessions."

#define HELP_EDITCARD "WARNING: ANY CHANGES YOU MAKE TO THE CARDS ARE \
PERMANENT! THERE IS NO WAY TO UNDO ANY CHANGES UNLESS *YOU* FIND AN \
ORIGINAL COPY OF THE SWCCGDM.DM DATA FILE! EDIT THE CARD AT YOUR OWN \
RISK! YOU ARE URGED TO HIT CANCEL NOW AND AVERT DISASTER! \
THE AUTHOR OF THIS PROGRAM IS IN NO WAY RESPONSIBLE \
FOR LOSS OF DATA OR EXPLAINING THE DETAILS OF THE ATTRIBUTES! THE \
ABILITY TO EDIT CARDS IS HERE AS A COURTESY! IT'S NOT YOUR RIGHT TO \
ALTER THE DATA AT WILL, IT'S A PRIVILEDGE THAT CAN BE TAKEN AWAY IF YOU \
HARRASS THE AUTHOR ABOUT IT. :) YOU'RE \
ON YOUR OWN! AGAIN, TO SAVE EVERYONE GRIEF AND HARDSHIP, PLEASE \
HIT CANCEL IF YOU DON'T KNOW EXACTLY WHAT YOU'RE DOING OR YOU \
AREN'T WILLING TO FACE THE DIRE CONSEQUENCES!!!! MOST OF ALL: \
DON'T SAY I DIDN'T WARN YOU!!!!!!\r\n\r\n\
To edit a card, fill out all the fields and hit CHANGE. \
The attributes field is a string of 15 characters with the following \
format:\r\n\
byte 0: destiny\r\n\
byte 1-2: type(*)\r\n\
byte 3: deploy\r\n\
byte 4: forfeit\r\n\
byte 5: power or LS force\r\n\
byte 6: ability or armor or defense or DS force\r\n\
byte 7: pilot skill(Y/N) or Hyper/Landspeed or Parsec\r\n\
byte 8: warrior skill(Y/N) or permanent pilot(Y/N)\r\n\
byte 9-10: rarity\r\n\
byte 11: set (see the file SWCCG Sets.dat)\r\n\
byte 12: side(L or D)\r\n\
byte 13: astromech(Y/N)\r\n\
byte 14: extra icon (D-Dejarik,A-Alien,W-Warrior,S-Ship,X-exterior,N-interior,etc.)\r\n\r\n\
(*)type field is always two characters:\r\n\
LP: system location\r\n\
LT: site location\r\n\
LM: mobile system\r\n\
LV: mobile site\r\n\
CR: rebel character\r\n\
CI: imperial character\r\n\
CA: alien character\r\n\
CD: droid character\r\n\
R0: creature\r\n\
VT: transport vehicle\r\n\
VS: shuttle vehicle\r\n\
VC: creature vehicle\r\n\
SC: capital starship\r\n\
SF: starfighter\r\n\
SS: starfighter squadron\r\n\
IU: used interrupt\r\n\
IL: lost interrupt\r\n\
IB: used or lost interrupt\r\n\
EE: effect\r\n\
EU: utinni effect\r\n\
EI: immediate effect\r\n\
D0: device\r\n\
WC: character weapon\r\n\
WS: starship weapon\r\n\
WA: automated weapon\r\n\
WD: death star weapon\r\n\
N0: epic event"

#define HELP_EDITVALUE "These card values are only estimates of a card's \
worth.  They may be too high or too low, depending on where you live, who \
you're buying them from, or the current market. As with any collectible, \
the true value is what the card is worth to you. The values change \
constantly, of course. If you don't have a current price guide, you can \
update the values yourself by entering it into the 'New' field under the \
card name. If you're changing many card values, you'll probably want to \
mention the source and the date of the source. To do this, check the box \
marked 'Edit Source'. IMPORTANT: If 'Edit Source' is unchecked when you hit \
OK, then the new source information won't be saved.\r\n\r\n\
To load a new price guide, copy the new 'SWCCG Price Guide.dm' file over \
the old one. The next time you run the DeckMaker, all the new prices will be \
used."

#define HELP_FIND "Enter in the box provided any combination of words \
contained in the card's lore, game text or name (depending on which of these \
fields is selected).  The results will be displayed in the stats window, \
where you can browse through them.  The searches are not case sensitive \
and any words preceded with a '-' will be excluded from the search.  For an \
exact phrase, surround the phrase with (\")s."

#define HELP_MASSENTRY "For various reasons, you may have a need to adjust all \
cards visible by a fixed amount. Here, you can enter which side to affect, \
what rarities, and what sets.  Changes are ONLY made to cards in the visible \
list. (ie, if white-bordered cards are combined, or one side is not shown, or \
a group of cards are excluded, those 'hidden' cards are not adjusted.) After \
choosing which side(s) to affect, the rarity level(s), and the set(s), enter a \
quantity to adjust.  This quantity is ADDED to the visible cards.  If you enter \
a negative number, this quantity is REMOVED from the visible cards.  (If you \
plan to remove cards, it's highly recommended to turn Combine White Border OFF \
unless you know what you're doing.)  All changes are permanent and cannot be \
undone, so you might want to save a backup of your deck before making mass \
card adjustments."

