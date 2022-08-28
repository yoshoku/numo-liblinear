# 2.2.0
- Update bundled LIBLINEAR to 2.45

# 2.1.1
- Refactor codes and configs with RuboCop.

# 2.1.0
- Update bundled LIBLINEAR to 2.44

# 2.0.0
- Redesign native extension codes.
- Change not ot use git submodule for LIBLINEAR codes bundle.
- Introduce conventional commits.

# 1.2.2
- Remove dependent gem's type declaration file from installation files.

# 1.2.1
- Fix version specifier of runtime dependencies.

# 1.2.0
- Add type declaration file: sig/numo/liblinear.rbs

# 1.1.2
- Add GC guard to model saving and loading methods.
- Fix size specification to memcpy function.

# 1.1.1
- Add GC guard codes.
- Fix some configuration files.

# 1.1.0
- Update bundled LIBLINEAR version to 2.4.1.
- Support one-class SVM implemented on LIBLINEAR ver. 2.4.0.

# 1.0.0
## Breaking change
- For easy installation, Numo::LIBLINEAR bundles LIBLINEAR codes.
There is no need to install LIBLINEAR in advance to use Numo::LIBLINEAR.

# 0.5.0
- Fix to use LIBLINEAR sparce vector representation for internal processing.

# 0.4.0
- Add verbose parameter to output learning process messages.
- Several documentation improvements.

# 0.3.0
- Add random_seed parameter for specifying seed to give to srand function.
- Several documentation improvements.

# 0.2.0
- Add validation of method parameters.
- Several documentation improvements.

# 0.1.0
- First release.
