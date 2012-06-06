memcache-tpl: an example libmemcached and TPL wrapper

Open source, but not an "open source project"
  "I am sharing my code. I am not launching an open source project." -- Alan Gutierrez

Notes
 - memcache_api.h is the end product, example.c/h and macrotest were iterative evolutions. Some of the previous iterations may not compile.
 - Does not appear to have a memory leak per my latest valgrind but may have some opportunities.  If so, send me a pull request!
 - Definitely has some opportunities for errno, return/"exception" checking.
 - This project is not thread-safe.
 - It now supports linked lists.
 - Now supports variadic parameters to the function macros.  This is for c# MAP expansion.
 - It hasn't been cleaned up.  It has a lot of opportunities but I think sharing it is more important than it being perfect.

License

    Copyright (c) 2012 Brian McManus

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
