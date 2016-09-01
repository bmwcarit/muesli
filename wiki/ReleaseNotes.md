#muesli 0.1.2
This is a bugfix release.
fixed:
* muesliConfig.cmake only creates rapidjson target once

#muesli 0.1.1
This is a bugfix release.
fixed:
* muesliConfig.cmake does not create GLOBAL imported targets
* JSON: allow to deserialize a double from an integer
* Local use of defines instead of global use via cmake

#muesli 0.1.0
This is the initial release of muesli.

Major features:
* Support polymorphic types.
* Support read/write to/from JSON.
* Optionally serialize nullable types (std::unique_ptr, std::shared_ptr, boost::optional).
* In-class or out-of class serialization.

