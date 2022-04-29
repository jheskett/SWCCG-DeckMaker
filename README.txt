Star Wars CCG DeckMaker Code Archive
------------------------------------

This repository contains a history of the DeckMaker program from approximately
when I took over maintenance (3.0) of the program (circa 1996) to when Jeff Heskett
returned to produce what I believe was the last version (3.5, probably somewhere
around 1998).

I was, at the time, also working on my own deck building software (WinDeck), which
unfortunately I have lost the source code to. I suspect some code from that program
made its way into this one, as I believe I added the ability to import data and decks
from WinDeck to DeckMaker. 

At any rate, WinDeck was a buggy mess that never really managed to be as elegant 
as the much simpler DeckMaker, so while it was popular for a time while Jeff 
was away from working on it, it's largely been forgotten now.

This copy is forked from a copy of the earlier revision on Jeff Heskett's own
github page at https://github.com/jheskett/SWCCG-DeckMaker -- you can find the same
git revisions there as at the `original` tag in this repository.

After this introduction, there is a brief explanation of the specifics of the history, 
in the form of the git log comments for each step, with irrelevant details stripped out.

- Megan (aka Sandworm, Inc. as a teenager in the late 90s).


tag: original, v3.0

    The original git repository pushed out by Jeff Heskett on his own github.
    This was the last version produced entirely by him.

    At the time, DeckMaker was the main tool used by people to build decks for
    SW:CCG.

tag: v3.2

    v3.2 Brief detour into C++ version.
    
    This is allegedly version 3.2, according to the main .cpp file. This would
    be the first version not completely written by Jeff. These files were just
    mingled in with the later C versions in my local copy, there was no version
    control for any of this at the time.
    
    I am unsure if this is a real released version or just a detour into a C++
    rewrite that didn't pan out. This is likely incomplete, as later changes
    to the header files would have broken it and there's no build files from
    this period.
    
    I believe, though, that I may have been working on it in C++ until Jeff Heskett
    returned to it later on, for the 3.5 release, which is the version I have complete
    files for.

tag: v3.5

    v3.5: Update everything to the last archived copy I had.
    
    This is probably the version Jeff worked on upon returning to working
    on DeckMaker briefly, around 1998 or so? This version returns back to
    being written in C rather than C++ (I think it wasn't a terribly difficult
    transition, with the C++ being rather meek).
    
    This version *might* build? I do not currently have the tools to verify.

commit ea307db85bbd7bc1823eb8a6d923be9b2bfafe99

    Add all the data files that were hanging around.
    
    These are set data files for each of the sets released at the time
    of this code dump. I believe this dates the dump to around 1999, with
    the release of the Dagobah set.

commit b1b2c9c8c417f4919f303705f3886f7b3ad07b2b

    Add some deck files I had in the directory as well for good measure.
    
    These are just some deck files that I probably used for testing. I'm
    honestly not sure if they're deckmaker files or windeck (my program's)
    files.
