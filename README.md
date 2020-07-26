> Currently app not work properly. To make it work, sqlite database should be manually created and hardcoded in `mainwindow.cpp`.

# Simple notebook

## General information
Main features of this app are import and export from/to VCF (vCard) format.
Format of exported VCF file corresponds to [RFC 6435 standart](https://tools.ietf.org/html/rfc6350).

Currently, only first name (`FN`), telephone number (`TEL`) and photo (`PHOTO`) properties are available. Other properties are ignored.

## Todo:
- [ ] **\*IMPORTANT\* Dynamically create database if any can't be found**
- [ ] Add support to as much properties as possible
