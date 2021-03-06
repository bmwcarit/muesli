# muesli 1.0.2

## API relevant changes
None.

## Other changes
* introduce compiler flags and fix warnings

# muesli 1.0.1

## API relevant changes
None.

## Other changes
* fix deserialization of unsigned integers introduced in 1.0.0

# muesli 1.0.0
API Stable

## API relevant changes
None.

## Other changes
* handle `NaN` in JSON: serialize to `null` / deserialize from `null`

# muesli 0.2.0

## API relevant changes
None.

## Other changes
* added support for std::set and std::unordered_set

# muesli 0.1.2
This is a bugfix release.
fixed:
* muesliConfig.cmake only creates rapidjson target once

# muesli 0.1.1
This is a bugfix release.
fixed:
* muesliConfig.cmake does not create GLOBAL imported targets
* JSON: allow to deserialize a double from an integer
* Local use of defines instead of global use via cmake

# muesli 0.1.0
This is the initial release of muesli.

Major features:
* Support polymorphic types.
* Support read/write to/from JSON.
* Optionally serialize nullable types (std::unique_ptr, std::shared_ptr, boost::optional).
* In-class or out-of class serialization.

