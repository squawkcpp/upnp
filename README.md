[![Build Status](https://travis-ci.org/squawkcpp/upnp.svg?branch=master)](https://travis-ci.org/squawkcpp/upnp)
[![GitHub version](https://badge.fury.io/gh/squawkcpp%2Fupnp.svg)](https://badge.fury.io/gh/squawkcpp%2Fupnp)

# Universal Plug and Play Server (UPNP)

Server to content directory to a UPNP network.

## Installation

### Install Static Package

A prebuild static package is available as github release:

<pre>
% wget https://github.com/squawkcpp/upnp/releases/download/&lt;TAG&gt;/cds_&lt;TAG&gt;.deb
% tar xfz upnp_&lt;TAG&gt;.tar.gz -C /
</pre>

### Use Docker Image

Install and run the docker image with the parameters for the cds server.

<pre>
sudo docker run -itd --link <REDIS> --name squawk-cds -v /srv:/srv:ro \
    -v TMP_DIRECTORY=/var/tmp/cds
    -p 9001:9001 \
    -e REDIS=<REDIS> \
    -e AMAZON_ACCESS_KEY=<ACCESS_KEY> \
    -e AMAZON_KEY=<KEY> \
    -e TMDB_KEY=<KEY> \
    -e DIRECTORY=/foo,/bar \
    -e TMP_DIRECTORY=/var/tmp/cds \
    squawk/cds:&lt;TAG&gt;
</pre>

The options are the same as for the command line, except the handling of directories.
Multiple directories are set as list with the path items separated by a comma. If you plan to reuse the
datas accross multiple images you will have to share the tmp directory.
Mount the tmp directory to the docker image and configure cds accordingly.

## Usage

```
cds [OPTION...]
```

##### options

name&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | value | description
------------ | ------------- | -------------
--directory|PATH|path to the directory with the mediafiles. Multiple entries will result in a list containing all directories.
--listen|IP|API Webserver IP-Adress to bind to.
--http-port|PORT|API Webserver IP Port to bind to.
--tmp-directory|PATH|temporary folder for the thumbnails.
--tmdb-key|KEY|API key for tmdb.
--amazon-access-key|KEY|Access key for amazon.
--amazon-key|KEY|API key for amazon.
--redis|HOST|Redis Database (default: localhost) (default: localhost)
--redis-port|PORT|Redis Database port (default: 6379) (default: 6379)
--help| |Print help

## Dependencies:

binary dependencies needs to be installed on the target system.


these libraries will be downloaded and staticaly linked during the compile process.

- [asio](http://think-async.com) is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach.
- [cxxopts](https://github.com/jarro2783/cxxopts), lightweight C++ command line option parser
- [format::fmt](http://fmtlib.net), small, safe and fast formatting library
- [rapidjson](http://rapidjson.org/), a fast JSON parser/generator for C++ with both SAX/DOM style API
- [RapidXML NS](https://github.com/svgpp/rapidxml_ns) library - RapidXML with added XML namespaces support
- [re2](https://github.com/google/re2) is a fast, safe, thread-friendly alternative to backtracking regular expression engines like those used in PCRE, Perl, and Python. It is a C++ library.
- [redox](https://github.com/hmartiro/redox), modern, asynchronous, and wicked fast C++11 client for Redis
- [spdlog](https://github.com/gabime/spdlog), super fast C++ logging library.

## Licence:

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

