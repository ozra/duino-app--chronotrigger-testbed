# Things To Do Next #

- Improve Makefile - specialize to reasonable generalization level.
- Rewrite modules to stand alone modules
   - name the dev/module pattern via a base class (IntertwinedFSMs or so)
   - container class which contains multiple of same mod type - for auto-iteration
   - single mod methods are delegated from iterating versions (`list.update_all` => `list.each \.update`)

