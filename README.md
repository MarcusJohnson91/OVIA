**OVIA:**
========
* OVIA is a project for Input/Output on Audio/Video streams.
* OVIA the project includes OVIA the library, and a utility (TrimSilence) for stripping empty samples from audio files (I may eventually add a function to trim empty columsn/rows of pixels as well, but it's not here yet).

License:
=======
OVIA is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.

Here's a tl;dr of my license:

* **Do** If you permanently fork OVIA, change the name to reduce confusion.
* **Do** include in your acknowledgments a link to [OVIA on GitLab](https://gitlab.com/BumbleBritches57/OVIA).
* **Don't** plagiarize OVIA.
* **Don't** relicense OVIA.
* **Don't** sue me if something goes wrong. I never have and never will guarantee OVIA works for anything, you figure out if it's right for you.

How To Use OVIA:
===================
In order to use OVIA, you need to include OVIA.h.

OVIA:
-----
* To use OVIA in `main()` call `AudioContainer_Init` or `ImageContainer_Init`.
* Init a `BitInput` and/or `BitOutput` file to set up a stream, and a `BitBuffer` to read from or write to the container.
* Call `OVIA_Identify` to figure out what kind of file is in your BitInput.
* Call `OVIA_Parse` in order to extract all the internal/external metadata.
