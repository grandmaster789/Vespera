# The Vespera project

A long time ago I was working on a step-by-step walkthrough of making a custom game engine, both to explain in detail the steps involved and to clarify the architecture to myself. This is a continuation of that work, where I return to diving into all the steps involved in making a fully custom game engine, with corresponding high performance targets.

Recently I've acquired a SteamFrame VR headset, which is a good target platform to keep in mind - VR has very tight constraints on the frame rate. I still would like to have pretty shiny graphics, so I'm going to target it via the streaming wifi adapter from a desktop computer. I'm treating this the way a solo developer would, which mostly has consequences for what the asset pipeline looks like - heavily lean on procedural generation, but I'd still like to be able to be in control and author stuff as needed. For VR the guideline is to achieve a framerate of 90 fps, which is challenging.

The theme of control also applies to the library stack we'll use - keep a pretty minimal list of dependencies, although I would like to be relatively flexible in what platform to target. Recently I've become aware of the shaped-core libraries and its new SGL shading language and this is a good opportunity to become acquainted with it. At the very least I'd like to target Windows and Linux platforms, which leads us to the following set of dependencies:
- vulkan
- shaped-core (which includes SGL)
- shaped-xr (for OpenXR support)
- OS API's  (for window, input and audio)

Everything else will be implemented by hand, which should be very informative. I'm going to use C++23 with all of the latest tricks that enables, which should be interesting and educational.

As for the game that will be developed on top of this engine - it's going ot be a VR-first bullet hell. A kind of mix between Tempest, Wipeout, Thumper and Returnal. It's something I haven't really seen before, and sounds pretty fun to me. Tempest has some excellent aesthetics that could use a more modern take, and I've always enjoyed the general atmosphere of Wipeout. Importantly, the models involved are relatively simple so we don't have to implement character animation and such. Thumper pioneered a kind of rhythmic pulsing gameplay that I'd like to try and replicate, with a sense of speed that was an excellent experience in VR. Finally the bullets as used in Returnal should make for an interesting challenge both in game design and in programming. I'm not entirely sure where this will end up, but it feels like this could be something good.

At this point we have a tech stack, a set of platform requirements and a rough game outline. Now the overall approach - I'd like to have a clean layered architecture for the game engine itself, but at the same time I'd like to have something playable as soon as possible. That's the approach - build minimal layers to achieve a vertical game slice, then widen as needed to improve the final product. I'm going to use AI models mostly to check my work and verify that it's going in the right direction, but all of the code will be hand-written so that I deeply understand everything that was written.

Enjoy the ride, we're going to have some fun here :)