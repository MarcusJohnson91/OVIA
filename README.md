**OVIA:**
========
* OVIA is multimedia framework; it's the LLVM of multimedia codecs and formats.
* The name is a pronouncable acronym for Input Output Audio Video.

License:
=======
* All contributions are accepted only under the terms of the public-domain and CC0 licenses.
* OVIA is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.
* OVIA's subproject LIVC is proprietary and is private, though the build files optionally reference LIVC for joined compilation.

Here's a tl;dr of my license:

* **Do** If you permanently fork OVIA, change the name to reduce confusion.
* **Do** include in your acknowledgments a link to [OVIA on GitHub](https://github.com/MarcusJohnson91/OVIA).
* **Don't** plagiarize OVIA.
* **Don't** relicense OVIA.
* **Don't** sue me if something goes wrong. I never have and never will guarantee OVIA works for anything, you figure out if it's right for you.

OVIA API:
=======
OVIA has two APIs, the Internal API, and the External or Public API.
** External API **:
* include OVIA.h to get access to the External API.
* The External API allows users of OVIA to open Files and Streams, identify their content, and extract/create new streams from various data sources.

** Internal API **:
The Internal API is composed of about half a dozen interfaces; those interfaces are: 
* CodecIO: Abstracts away differences in Codecs and provides a consistent interface to encode/decode various codecs.
* EntropyIO: Provides various entropy coders to increase code sharing between codecs.
* TagIO: Provides a consistent interface for various metadata systems in use by various codecs/formats.
* StreamIO: Provides a consistent interface for packet and chunk based formats.
* MediaIO: Provides an abstraction to contain audio samples and image frames.

Dependencies:
===========
OVIA depends on FoundationIO for various subsystems, like Text handling (especially Unicode), File/Network access, bit reading/writing, and more.
* So either clone OVIA with the `--recurse-submodules` flag, or  if you've already cloned it you can use `git submodule update --init --recursive` to clone FoundationIO.
* OVIA also includes a submodule containing the proprietary LIVC project that won't be checked out without the correct access rights; ignore any warnings about it.
