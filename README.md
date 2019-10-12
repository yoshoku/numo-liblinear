# Numo::Liblinear

[![Build Status](https://travis-ci.org/yoshoku/numo-liblinear.svg?branch=master)](https://travis-ci.org/yoshoku/numo-liblinear)
[![Gem Version](https://badge.fury.io/rb/numo-liblinear.svg)](https://badge.fury.io/rb/numo-liblinear)
[![BSD 3-Clause License](https://img.shields.io/badge/License-BSD%203--Clause-orange.svg)](https://github.com/yoshoku/numo-liblinear/blob/master/LICENSE.txt)
[![Documentation](http://img.shields.io/badge/docs-rdoc.info-blue.svg)](https://www.rubydoc.info/gems/numo-liblinear/0.4.0)

Numo::Liblinear is a Ruby gem binding to the [LIBLINEAR](https://www.csie.ntu.edu.tw/~cjlin/liblinear/) library.
LIBLINEAR is one of the famous libraries for large-scale regularized linear classification and regression.
Numo::Liblinear makes to use the LIBLINEAR functions with
dataset represented by [Numo::NArray](https://github.com/ruby-numo/numo-narray).

Note: There are other useful Ruby gems binding to LIBLINEAR:
[liblinear-ruby](https://github.com/kei500/liblinear-ruby) by Kei Tsuchiya and
[liblinear-ruby-swig](https://github.com/tomz/liblinear-ruby-swig) by Tom Zeng.

## Installation
Numo::Liblinear bundles LIBLINEAR. There is no need to install LIBLINEAR in advance.

Add this line to your application's Gemfile:

```ruby
gem 'numo-liblinear'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install numo-liblinear

## Usage

### Preparation

In the following examples, we use [red-datasets](https://github.com/red-data-tools/red-datasets) to download dataset.

    $ gem install red-datasets-numo-narray

### Example 1. Cross-validation

We conduct cross validation of the Support Vector Classifier on [Iris dataset](https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/multiclass.html#iris).

```ruby
require 'numo/narray'
require 'numo/liblinear'
require 'datasets-numo-narray'

# Download Iris dataset.
puts 'Download dataset.'
iris = Datasets::LIBSVM.new('iris').to_narray
x = iris[true, 1..-1]
y = iris[true, 0]

# Define parameters of L2-regularized L2-loss support vector classification.
param = {
  solver_type: Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL,
  C: 1
}

# Perform 5-cross validation.
puts 'Perform cross validation.'
n_folds = 5
predicted = Numo::Liblinear::cv(x, y, param, n_folds)

# Print mean accuracy.
mean_accuracy = y.eq(predicted).count.fdiv(y.size)
puts "Accuracy: %.1f %%" % (100 * mean_accuracy)
```

Execution result in the following:

```sh
Download dataset.
Perform cross validation.
Accuracy: 87.3 %
```

### Example 2. Pendigits dataset classification

We first train the Logistic Regression using training [pendigits dataset](https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/multiclass.html#pendigits).

```ruby
require 'numo/liblinear'
require 'datasets-numo-narray'

# Download pendigits training dataset.
puts 'Download dataset.'
pendigits = Datasets::LIBSVM.new('pendigits').to_narray
x = pendigits[true, 1..-1]
y = pendigits[true, 0]

# Define parameters of L2-regularized logistic regression.
param = {
  solver_type: Numo::Liblinear::SolverType::L2R_LR_DUAL,
  C: 1
}

# Perform training procedure.
puts 'Train logistic regression.'
model = Numo::Liblinear.train(x, y, param)

# Save parameters and trained model.
puts 'Save parameters and model with Marshal'
File.open('pendigits.dat', 'wb') { |f| f.write(Marshal.dump([param, model])) }
```

```sh
Download dataset.
Train logistic regression.
Save parameters and model with Marshal
```

We then predict labels of testing dataset, and evaluate the classifier.

```ruby
require 'numo/liblinear'
require 'datasets-numo-narray'

# Download pendigits testing dataset.
puts 'Download dataset.'
pendigits_test = Datasets::LIBSVM.new('pendigits', note: 'testing').to_narray
x = pendigits_test[true, 1..-1]
y = pendigits_test[true, 0]

# Load parameter and model.
puts 'Load parameter and model.'
param, model = Marshal.load(File.binread('pendigits.dat'))

# Predict labels.
puts 'Predict labels.'
predicted = Numo::Liblinear.predict(x, param, model)

# Evaluate classification results.
mean_accuracy = y.eq(predicted).count.fdiv(y.size)
puts "Accuracy: %.1f %%" % (100 * mean_accuracy)
```

```sh
Download dataset.
Load parameter and model.
Predict labels.
Accuracy: 87.9 %
```

## Note
The hyperparemter of LIBLINEAR is given with Ruby Hash on Numo::Liblinear.
The hash key of hyperparameter and its meaning match the struct parameter of LIBLINEAR.
The parameter is detailed in [LIBLINEAR README](https://github.com/cjlin1/liblinear/blob/master/README)

```ruby
param = {
  solver_type:                    # [Integer] Type of Solver
    Numo::Liblinear::SolverType::L2R_L2LOSS_SVC_DUAL,
  eps: 0.01,                      # [Float] Stopping criterion
  C: 1,                           # [Float] Cost of constraints violation
  nr_weight: 3,                   # [Integer] Number of weights
  weight_label:                   # [Numo::Int32] Labels to add weight
    Numo::Int32[0, 1, 2],
  weight:                         # [Numo::DFloat] Weight values
    Numo::DFloat[0.4, 0.4, 0.2],
  p: 0.1,                         # [Float] Sensitiveness of loss of support vector regression
  verbose: false,                 # [Boolean] Whether to output learning process message
  random_seed: 1                  # [Integer/Nil] Random seed
}
```

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake spec` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/yoshoku/numo-liblinear.
This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [Contributor Covenant](http://contributor-covenant.org) code of conduct.

## Code of Conduct

Everyone interacting in the Numo::Liblinear project’s codebases, issue trackers, chat rooms and mailing lists is expected to follow
the [code of conduct](https://github.com/yoshoku/numo-liblinear/blob/master/CODE_OF_CONDUCT.md).
