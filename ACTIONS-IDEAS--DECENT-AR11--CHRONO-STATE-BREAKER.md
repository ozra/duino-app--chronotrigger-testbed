# Things To Do Next #

- MQL3T - MQTT LighT for arduino ("bvte code" the topic slugs)
   - alloc model: each slug is stored with 1B len first (then we know when there's  a free slot, when it is 0 [but last used pos should be stored ofc...]
   - ascii only topic chars


- A unique package format will likely be used (some _subpart_ of decent) - G3 - using GOGS,GITHUB,GX
   - "spotrunner" / "packing-heat"
- If _no_ temperature sensors are found: fatal()
- Improve Makefile - specialize to reasonable generalization level.
   - integrate with "less-confuzed" & "skelett"
- Rewrite modules to stand alone modules
   - name the dev/module pattern via a base class (IntertwinedFSMs or so)
   - container class which contains multiple of same mod type - for auto-iteration
   - single mod methods are delegated from iterating versions (`list.update_all` => `list.each \.update`)

- Add `Profiler` / `TimingStats` singletonish class, for pinpointing time consumption problems.
   + Number of different timings in template param
   + start(n), stop(n) for timing
   + stores avg-t, max-t and total-t for each timing index (avg = ema)
      ```
      type Profile => @max-t, @min-t, @ema300-t
      prm = new ProfileManager<Profile, 6>();

      init()->
         prm.log-interval(5.seconds)

      loop()->
         prm.start(1)
         do-stuff()
         prm.start(2)
         do-other-stuff()
         prm.mind-log()
      ```

