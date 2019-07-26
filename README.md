# Numo::Liblinear

[![Build Status](https://travis-ci.org/yoshoku/numo-liblinear.svg?branch=master)](https://travis-ci.org/yoshoku/numo-liblinear)
[![BSD 3-Clause License](https://img.shields.io/badge/License-BSD%203--Clause-orange.svg)](https://github.com/yoshoku/numo-liblinear/blob/master/LICENSE.txt)

Numo::Liblinear is a Ruby gem binding to the [LIBLINEAR](https://www.csie.ntu.edu.tw/~cjlin/liblinear/) library.
LIBLINEAR is one of the famous libraries for large-scale regularized linear classification and regression.
Numo::Liblinear makes to use the LIBLINEAR functions with
dataset represented by [Numo::NArray](https://github.com/ruby-numo/numo-narray).

Note: There are other useful Ruby gems binding to LIBLINEAR:
[liblinear-ruby](https://github.com/kei500/liblinear-ruby) by Kei Tsuchiya and
[liblinear-ruby-swig](https://github.com/tomz/liblinear-ruby-swig) by Tom Zeng.

## Installation
Numo::Liblinear does not bundle LIBLINEAR. You need to install LIBLINEAR in advance along your environment.

macOS:

    $ brew install liblinear

Ubuntu:

    $ sudo apt-get install liblinear-dev

Add this line to your application's Gemfile:

```ruby
gem 'numo-liblinear'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install numo-liblinear

## Usage

TODO: Write usage instructions here

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake spec` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/yoshoku/numo-liblinear.
This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [Contributor Covenant](http://contributor-covenant.org) code of conduct.

## Code of Conduct

Everyone interacting in the Numo::Liblinear project’s codebases, issue trackers, chat rooms and mailing lists is expected to follow
the [code of conduct](https://github.com/yoshoku/numo-liblinear/blob/master/CODE_OF_CONDUCT.md).
