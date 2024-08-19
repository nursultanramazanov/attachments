# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# This workflow starts your API and fuzzes it with ForAllSecure Mayhem for API
# to find reliability, performance and security issues before they reach
# production.
#
# To use this workflow, you will need to:
#
# 1. Create a Mayhem account at https://app.mayhem.security
#
# 2. Create an API token at https://app.mayhem.security/-/settings/user/api-tokens
#
# 3. Add the API token as a secret in GitHub called "MAYHEM_TOKEN"
#
# 4. Update the "Start your API" step to run your API in the background before
#    starting the Mayhem for API scan, and update the `api-url` & `api-spec`
#    field.
#
# If you have any questions, please contact us at mayhem4api@forallsecure.com

name: "Mayhem for API"

on: //
//  ActivityObserver.h
//  Clock Signal
//
//  
//  
//

#pragma once

#include <cstdint>
#include <string>

namespace Activity {

/*!
	Provides a purely virtual base class for anybody that wants to receive notifications of
	'activity': any feedback from an emulated system which a user could perceive other than
	through the machine's native audio and video outputs.

	So: status LEDs, drive activity, etc. A receiver may choose to make appropriate noises
	and/or to show or unshow status indicators.
*/
class Observer {
	public:
		virtual ~Observer() = default;

		/// Provides hints as to the sort of information presented on an LED.
		enum LEDPresentation: uint8_t {
			/// This LED informs the user of some sort of persistent state, e.g. scroll lock.
			/// If this flag is absent then the LED describes an ephemeral state, such as media access.
			Persistent = (1 << 0),
		};

		/// Announces to the receiver that there is an LED of name @c name.
		virtual void register_led([[maybe_unused]] const std::string &name, [[maybe_unused]] uint8_t presentation = 0) {}

		/// Announces to the receiver that there is a drive of name @c name.
		///
		/// If a drive has the same name as an LED, that LED goes with this drive.
		virtual void register_drive([[maybe_unused]] const std::string &name) {}

		/// Informs the receiver of the new state of the LED with name @c name.
		virtual void set_led_status([[maybe_unused]] const std::string &name, [[maybe_unused]] bool lit) {}

		enum class DriveEvent {
			StepNormal,
			StepBelowZero,
			StepBeyondMaximum
		};

		/// Informs the receiver that the named event just occurred for the drive with name @c name.
		virtual void announce_drive_event([[maybe_unused]] const std::string &name, [[maybe_unused]] DriveEvent event) {}

		/// Informs the receiver of the motor-on status of the drive with name @c name.
		virtual void set_drive_motor_status([[maybe_unused]] const std::string &name, [[maybe_unused]] bool is_on) {}
};

}
  push: //
//  ActivitySource.h
//  Clock Signal
//
//  
//  
//

#pragma once

#include "Observer.hpp"

namespace Activity {

class Source {
	public:
		virtual void set_activity_observer(Observer *observer) = 0;
};

}
    branches: [ "main" ]
  pull_request: //
//  MultiConfigurable.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiConfigurable.hpp"

#include <algorithm>

using namespace Analyser::Dynamic;

namespace {

class MultiStruct: public Reflection::Struct {
	public:
		MultiStruct(const std::vector<Configurable::Device *> &devices) : devices_(devices) {
			for(auto device: devices) {
				options_.emplace_back(device->get_options());
			}
		}

		void apply() {
			auto options = options_.begin();
			for(auto device: devices_) {
				device->set_options(*options);
				++options;
			}
		}

		std::vector<std::string> all_keys() const final {
			std::set<std::string> keys;
			for(auto &options: options_) {
				const auto new_keys = options->all_keys();
				keys.insert(new_keys.begin(), new_keys.end());
			}
			return std::vector<std::string>(keys.begin(), keys.end());
		}

		std::vector<std::string> values_for(const std::string &name) const final {
			std::set<std::string> values;
			for(auto &options: options_) {
				const auto new_values = options->values_for(name);
				values.insert(new_values.begin(), new_values.end());
			}
			return std::vector<std::string>(values.begin(), values.end());
		}

		const std::type_info *type_of(const std::string &name) const final {
			for(auto &options: options_) {
				auto info = options->type_of(name);
				if(info) return info;
			}
			return nullptr;
		}

		size_t count_of(const std::string &name) const final {
			for(auto &options: options_) {
				auto info = options->type_of(name);
				if(info) return options->count_of(name);
			}
			return 0;
		}

		const void *get(const std::string &name) const final {
			for(auto &options: options_) {
				auto value = options->get(name);
				if(value) return value;
			}
			return nullptr;
		}

		void *get(const std::string &name) final {
			for(auto &options: options_) {
				auto value = options->get(name);
				if(value) return value;
			}
			return nullptr;
		}

		void set(const std::string &name, const void *value, size_t offset) final {
			const auto safe_type = type_of(name);
			if(!safe_type) return;

			// Set this property only where the child's type is the same as that
			// which was returned from here for type_of.
			for(auto &options: options_) {
				const auto type = options->type_of(name);
				if(!type) continue;

				if(*type == *safe_type) {
					options->set(name, value, offset);
				}
			}
		}

	private:
		const std::vector<Configurable::Device *> &devices_;
		std::vector<std::unique_ptr<Reflection::Struct>> options_;
};

}

MultiConfigurable::MultiConfigurable(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines) {
	for(const auto &machine: machines) {
		Configurable::Device *device = machine->configurable_device();
		if(device) devices_.push_back(device);
	}
}

void MultiConfigurable::set_options(const std::unique_ptr<Reflection::Struct> &str) {
	const auto options = dynamic_cast<MultiStruct *>(str.get());
	options->apply();
}

std::unique_ptr<Reflection::Struct> MultiConfigurable::get_options() {
	return std::make_unique<MultiStruct>(devices_);
}
    # The branches below must be a subset of the branches above
    branches: [ "main" ]

jobs: //
//  MultiConfigurable.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../../Machines/DynamicMachine.hpp"
#include "../../../../Configurable/Configurable.hpp"

#include <memory>
#include <vector>

namespace Analyser::Dynamic {

/*!
	Provides a class that multiplexes the configurable interface to multiple machines.

	Makes a static internal copy of the list of machines; makes no guarantees about the
	order of delivered messages.
*/
class MultiConfigurable: public Configurable::Device {
	public:
		MultiConfigurable(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines);

		// Below is the standard Configurable::Device interface; see there for documentation.
		void set_options(const std::unique_ptr<Reflection::Struct> &options) final;
		std::unique_ptr<Reflection::Struct> get_options() final;

	private:
		std::vector<Configurable::Device *> devices_;
};

}
  mayhem-for-api: //
//  MultiJoystickMachine.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiJoystickMachine.hpp"

#include <algorithm>

using namespace Analyser::Dynamic;

namespace {

class MultiJoystick: public Inputs::Joystick {
	public:
		MultiJoystick(std::vector<MachineTypes::JoystickMachine *> &machines, std::size_t index) {
			for(const auto &machine: machines) {
				const auto &joysticks = machine->get_joysticks();
				if(joysticks.size() >= index) {
					joysticks_.push_back(joysticks[index].get());
				}
			}
		}

		const std::vector<Input> &get_inputs() final {
			if(inputs.empty()) {
				for(const auto &joystick: joysticks_) {
					std::vector<Input> joystick_inputs = joystick->get_inputs();
					for(const auto &input: joystick_inputs) {
						if(std::find(inputs.begin(), inputs.end(), input) != inputs.end()) {
							inputs.push_back(input);
						}
					}
				}
			}

			return inputs;
		}

		void set_input(const Input &digital_input, bool is_active) final {
			for(const auto &joystick: joysticks_) {
				joystick->set_input(digital_input, is_active);
			}
		}

		void set_input(const Input &digital_input, float value) final {
			for(const auto &joystick: joysticks_) {
				joystick->set_input(digital_input, value);
			}
		}

		void reset_all_inputs() final {
			for(const auto &joystick: joysticks_) {
				joystick->reset_all_inputs();
			}
		}

	private:
		std::vector<Input> inputs;
		std::vector<Inputs::Joystick *> joysticks_;
};

}

MultiJoystickMachine::MultiJoystickMachine(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines) {
	std::size_t total_joysticks = 0;
	std::vector<MachineTypes::JoystickMachine *> joystick_machines;
	for(const auto &machine: machines) {
		auto joystick_machine = machine->joystick_machine();
		if(joystick_machine) {
			joystick_machines.push_back(joystick_machine);
			total_joysticks = std::max(total_joysticks, joystick_machine->get_joysticks().size());
		}
	}

	for(std::size_t index = 0; index < total_joysticks; ++index) {
		joysticks_.emplace_back(new MultiJoystick(joystick_machines, index));
	}
}

const std::vector<std::unique_ptr<Inputs::Joystick>> &MultiJoystickMachine::get_joysticks() {
	return joysticks_;
}
    name: //
//  MultiJoystickMachine.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../../Machines/DynamicMachine.hpp"

#include <memory>
#include <vector>

namespace Analyser::Dynamic {

/*!
	Provides a class that multiplexes the joystick machine interface to multiple machines.

	Makes a static internal copy of the list of machines; makes no guarantees about the
	order of delivered messages.
*/
class MultiJoystickMachine: public MachineTypes::JoystickMachine {
	public:
		MultiJoystickMachine(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines);

		// Below is the standard JoystickMachine::Machine interface; see there for documentation.
		const std::vector<std::unique_ptr<Inputs::Joystick>> &get_joysticks() final;

	private:
		std::vector<std::unique_ptr<Inputs::Joystick>> joysticks_;
};

}
    # Mayhem for API runs on linux, mac and windows
    runs-on: //
//  MultiKeyboardMachine.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiKeyboardMachine.hpp"

using namespace Analyser::Dynamic;

MultiKeyboardMachine::MultiKeyboardMachine(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines) {
	for(const auto &machine: machines) {
		auto keyboard_machine = machine->keyboard_machine();
		if(keyboard_machine) machines_.push_back(keyboard_machine);
	}
	keyboard_ = std::make_unique<MultiKeyboard>(machines_);
}

void MultiKeyboardMachine::clear_all_keys() {
	for(const auto &machine: machines_) {
		machine->clear_all_keys();
	}
}

void MultiKeyboardMachine::set_key_state(uint16_t key, bool is_pressed) {
	for(const auto &machine: machines_) {
		machine->set_key_state(key, is_pressed);
	}
}

void MultiKeyboardMachine::type_string(const std::string &string) {
	for(const auto &machine: machines_) {
		machine->type_string(string);
	}
}

bool MultiKeyboardMachine::can_type(char c) const {
	bool can_type = true;
	for(const auto &machine: machines_) {
		can_type &= machine->can_type(c);
	}
	return can_type;
}

Inputs::Keyboard &MultiKeyboardMachine::get_keyboard() {
	return *keyboard_;
}

MultiKeyboardMachine::MultiKeyboard::MultiKeyboard(const std::vector<::MachineTypes::KeyboardMachine *> &machines)
	: machines_(machines) {
	for(const auto &machine: machines_) {
		observed_keys_.insert(machine->get_keyboard().observed_keys().begin(), machine->get_keyboard().observed_keys().end());
		is_exclusive_ |= machine->get_keyboard().is_exclusive();
	}
}

bool MultiKeyboardMachine::MultiKeyboard::set_key_pressed(Key key, char value, bool is_pressed, bool is_repeat) {
	bool was_consumed = false;
	for(const auto &machine: machines_) {
		was_consumed |= machine->get_keyboard().set_key_pressed(key, value, is_pressed, is_repeat);
	}
	return was_consumed;
}

void MultiKeyboardMachine::MultiKeyboard::reset_all_keys() {
	for(const auto &machine: machines_) {
		machine->get_keyboard().reset_all_keys();
	}
}

const std::set<Inputs::Keyboard::Key> &MultiKeyboardMachine::MultiKeyboard::observed_keys() const {
	return observed_keys_;
}

bool MultiKeyboardMachine::MultiKeyboard::is_exclusive() const {
	return is_exclusive_;
}
    permissions: //
//  MultiKeyboardMachine.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../../Machines/DynamicMachine.hpp"
#include "../../../../Machines/KeyboardMachine.hpp"

#include <memory>
#include <vector>

namespace Analyser::Dynamic {

/*!
	Provides a class that multiplexes the keyboard machine interface to multiple machines.

	Makes a static internal copy of the list of machines; makes no guarantees about the
	order of delivered messages.
*/
class MultiKeyboardMachine: public MachineTypes::KeyboardMachine {
	private:
		std::vector<MachineTypes::KeyboardMachine *> machines_;

		class MultiKeyboard: public Inputs::Keyboard {
			public:
				MultiKeyboard(const std::vector<MachineTypes::KeyboardMachine *> &machines);

				bool set_key_pressed(Key key, char value, bool is_pressed, bool is_repeat) final;
				void reset_all_keys() final;
				const std::set<Key> &observed_keys() const final;
				bool is_exclusive() const final;

			private:
				const std::vector<MachineTypes::KeyboardMachine *> &machines_;
				std::set<Key> observed_keys_;
				bool is_exclusive_ = false;
		};
		std::unique_ptr<MultiKeyboard> keyboard_;

	public:
		MultiKeyboardMachine(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines);

		// Below is the standard KeyboardMachine::Machine interface; see there for documentation.
		void clear_all_keys() final;
		void set_key_state(uint16_t key, bool is_pressed) final;
		void type_string(const std::string &) final;
		bool can_type(char c) const final;
		Inputs::Keyboard &get_keyboard() final;
};

}
      actions: //
//  MultiMediaTarget.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiMediaTarget.hpp"

using namespace Analyser::Dynamic;

MultiMediaTarget::MultiMediaTarget(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines) {
	for(const auto &machine: machines) {
		auto media_target = machine->media_target();
		if(media_target) targets_.push_back(media_target);
	}
}

bool MultiMediaTarget::insert_media(const Analyser::Static::Media &media) {
	bool inserted = false;
	for(const auto &target : targets_) {
		inserted |= target->insert_media(media);
	}
	return inserted;
}
      contents: //
//  MultiMediaTarget.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../../Machines/MediaTarget.hpp"
#include "../../../../Machines/DynamicMachine.hpp"

#include <memory>
#include <vector>

namespace Analyser::Dynamic {

/*!
	Provides a class that multiplexes the media target interface to multiple machines.

	Makes a static internal copy of the list of machines; makes no guarantees about the
	order of delivered messages.
*/
struct MultiMediaTarget: public MachineTypes::MediaTarget {
	public:
		MultiMediaTarget(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines);

		// Below is the standard MediaTarget::Machine interface; see there for documentation.
		bool insert_media(const Analyser::Static::Media &media) final;

	private:
		std::vector<MachineTypes::MediaTarget *> targets_;
};

}
      security-events: //
//  MultiProducer.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiProducer.hpp"

#include <condition_variable>
#include <mutex>

using namespace Analyser::Dynamic;

// MARK: - MultiInterface

template <typename MachineType>
void MultiInterface<MachineType>::perform_parallel(const std::function<void(MachineType *)> &function) {
	// Apply a blunt force parallelisation of the machines; each run_for is dispatched
	// to a separate queue and this queue will block until all are done.
	volatile std::size_t outstanding_machines;
	std::condition_variable condition;
	std::mutex mutex;
	{
		std::lock_guard machines_lock(machines_mutex_);
		std::lock_guard lock(mutex);
		outstanding_machines = machines_.size();

		for(std::size_t index = 0; index < machines_.size(); ++index) {
			const auto machine = ::Machine::get<MachineType>(*machines_[index].get());
			queues_[index].enqueue([&mutex, &condition, machine, function, &outstanding_machines]() {
				if(machine) function(machine);

				std::lock_guard lock(mutex);
				outstanding_machines--;
				condition.notify_all();
			});
		}
	}

	std::unique_lock lock(mutex);
	condition.wait(lock, [&outstanding_machines] { return !outstanding_machines; });
}

template <typename MachineType>
void MultiInterface<MachineType>::perform_serial(const std::function<void(MachineType *)> &function) {
	std::lock_guard machines_lock(machines_mutex_);
	for(const auto &machine: machines_) {
		const auto typed_machine = ::Machine::get<MachineType>(*machine.get());
		if(typed_machine) function(typed_machine);
	}
}

// MARK: - MultiScanProducer
void MultiScanProducer::set_scan_target(Outputs::Display::ScanTarget *scan_target) {
	scan_target_ = scan_target;

	std::lock_guard machines_lock(machines_mutex_);
	const auto machine = machines_.front()->scan_producer();
	if(machine) machine->set_scan_target(scan_target);
}

Outputs::Display::ScanStatus MultiScanProducer::get_scan_status() const {
	std::lock_guard machines_lock(machines_mutex_);
	const auto machine = machines_.front()->scan_producer();
	if(machine) return machine->get_scan_status();
	return Outputs::Display::ScanStatus();
}

void MultiScanProducer::did_change_machine_order() {
	if(scan_target_) scan_target_->will_change_owner();

	perform_serial([](MachineTypes::ScanProducer *machine) {
		machine->set_scan_target(nullptr);
	});
	std::lock_guard machines_lock(machines_mutex_);
	const auto machine = machines_.front()->scan_producer();
	if(machine) machine->set_scan_target(scan_target_);
}

// MARK: - MultiAudioProducer
MultiAudioProducer::MultiAudioProducer(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines, std::recursive_mutex &machines_mutex) : MultiInterface(machines, machines_mutex) {
	speaker_ = MultiSpeaker::create(machines);
}

Outputs::Speaker::Speaker *MultiAudioProducer::get_speaker() {
	return speaker_;
}

void MultiAudioProducer::did_change_machine_order() {
	if(speaker_) {
		speaker_->set_new_front_machine(machines_.front().get());
	}
}

// MARK: - MultiTimedMachine

void MultiTimedMachine::run_for(Time::Seconds duration) {
	perform_parallel([duration](::MachineTypes::TimedMachine *machine) {
		if(machine->get_confidence() >= 0.01f) machine->run_for(duration);
	});

	if(delegate_) delegate_->did_run_machines(this);
}
    steps: //
//  MultiProducer.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../../Concurrency/AsyncTaskQueue.hpp"
#include "../../../../Machines/MachineTypes.hpp"
#include "../../../../Machines/DynamicMachine.hpp"

#include "MultiSpeaker.hpp"

#include <memory>
#include <mutex>
#include <vector>

namespace Analyser::Dynamic {

template <typename MachineType> class MultiInterface {
	public:
		MultiInterface(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines, std::recursive_mutex &machines_mutex) :
			machines_(machines), machines_mutex_(machines_mutex), queues_(machines.size()) {}

	protected:
		/*!
			Performs a parallel for operation across all machines, performing the supplied
			function on each and returning only once all applications have completed.

			No guarantees are extended as to which thread operations will occur on.
		*/
		void perform_parallel(const std::function<void(MachineType *)> &);

		/*!
			Performs a serial for operation across all machines, performing the supplied
			function on each on the calling thread.
		*/
		void perform_serial(const std::function<void(MachineType *)> &);

	protected:
		const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines_;
		std::recursive_mutex &machines_mutex_;

	private:
		std::vector<Concurrency::AsyncTaskQueue<true>> queues_;
};

class MultiTimedMachine: public MultiInterface<MachineTypes::TimedMachine>, public MachineTypes::TimedMachine {
	public:
		using MultiInterface::MultiInterface;

		/*!
			Provides a mechanism by which a delegate can be informed each time a call to run_for has
			been received.
		*/
		struct Delegate {
			virtual void did_run_machines(MultiTimedMachine *) = 0;
		};
		/// Sets @c delegate as the receiver of delegate messages.
		void set_delegate(Delegate *delegate) {
			delegate_ = delegate;
		}

		void run_for(Time::Seconds duration) final;

	private:
		void run_for(const Cycles) final {}
		Delegate *delegate_ = nullptr;
};

class MultiScanProducer: public MultiInterface<MachineTypes::ScanProducer>, public MachineTypes::ScanProducer {
	public:
		using MultiInterface::MultiInterface;

		/*!
			Informs the MultiScanProducer that the order of machines has changed; it
			uses this as an opportunity to synthesis any CRTMachine::Machine::Delegate messages that
			are necessary to bridge the gap between one machine and the next.
		*/
		void did_change_machine_order();

		void set_scan_target(Outputs::Display::ScanTarget *scan_target) final;
		Outputs::Display::ScanStatus get_scan_status() const final;

	private:
		Outputs::Display::ScanTarget *scan_target_ = nullptr;
};

class MultiAudioProducer: public MultiInterface<MachineTypes::AudioProducer>, public MachineTypes::AudioProducer {
	public:
		MultiAudioProducer(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines, std::recursive_mutex &machines_mutex);

		/*!
			Informs the MultiAudio that the order of machines has changed; it
			uses this as an opportunity to switch speaker delegates as appropriate.
		*/
		void did_change_machine_order();

		Outputs::Speaker::Speaker *get_speaker() final;

	private:
		MultiSpeaker *speaker_ = nullptr;
};

/*!
	Provides a class that multiplexes the CRT machine interface to multiple machines.

	Keeps a reference to the original vector of machines; will access it only after
	acquiring a supplied mutex. The owner should also call did_change_machine_order()
	if the order of machines changes.
*/

}
      - uses: //
//  MultiSpeaker.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiSpeaker.hpp"

using namespace Analyser::Dynamic;

MultiSpeaker *MultiSpeaker::create(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines) {
	std::vector<Outputs::Speaker::Speaker *> speakers;
	for(const auto &machine: machines) {
		Outputs::Speaker::Speaker *speaker = machine->audio_producer()->get_speaker();
		if(speaker) speakers.push_back(speaker);
	}
	if(speakers.empty()) return nullptr;

	return new MultiSpeaker(speakers);
}

MultiSpeaker::MultiSpeaker(const std::vector<Outputs::Speaker::Speaker *> &speakers) :
	speakers_(speakers), front_speaker_(speakers.front()) {
	for(const auto &speaker: speakers_) {
		speaker->set_delegate(this);
	}
}

float MultiSpeaker::get_ideal_clock_rate_in_range(float minimum, float maximum) {
	float ideal = 0.0f;
	for(const auto &speaker: speakers_) {
		ideal += speaker->get_ideal_clock_rate_in_range(minimum, maximum);
	}

	return ideal / float(speakers_.size());
}

void MultiSpeaker::set_computed_output_rate(float cycles_per_second, int buffer_size, bool stereo) {
	stereo_output_ = stereo;
	for(const auto &speaker: speakers_) {
		speaker->set_computed_output_rate(cycles_per_second, buffer_size, stereo);
	}
}

bool MultiSpeaker::get_is_stereo() {
	// Return as stereo if any subspeaker is stereo.
	for(const auto &speaker: speakers_) {
		if(speaker->get_is_stereo()) {
			return true;
		}
	}
	return false;
}

void MultiSpeaker::set_output_volume(float volume) {
	for(const auto &speaker: speakers_) {
		speaker->set_output_volume(volume);
	}
}

void MultiSpeaker::speaker_did_complete_samples(Speaker *speaker, const std::vector<int16_t> &buffer) {
	auto delegate = delegate_.load(std::memory_order::memory_order_relaxed);
	if(!delegate) return;
	{
		std::lock_guard lock_guard(front_speaker_mutex_);
		if(speaker != front_speaker_) return;
	}
	did_complete_samples(this, buffer, stereo_output_);
}

void MultiSpeaker::speaker_did_change_input_clock(Speaker *speaker) {
	auto delegate = delegate_.load(std::memory_order::memory_order_relaxed);
	if(!delegate) return;
	{
		std::lock_guard lock_guard(front_speaker_mutex_);
		if(speaker != front_speaker_) return;
	}
	delegate->speaker_did_change_input_clock(this);
}

void MultiSpeaker::set_new_front_machine(::Machine::DynamicMachine *machine) {
	{
		std::lock_guard lock_guard(front_speaker_mutex_);
		front_speaker_ = machine->audio_producer()->get_speaker();
	}
	auto delegate = delegate_.load(std::memory_order::memory_order_relaxed);
	if(delegate) {
		delegate->speaker_did_change_input_clock(this);
	}
}

      # Run your API in the background. Ideally, the API would run in debug
      # mode & send stacktraces back on "500 Internal Server Error" responses
      # (don't do this in production though!)
      - name: //
//  MultiSpeaker.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../../Machines/DynamicMachine.hpp"
#include "../../../../Outputs/Speaker/Speaker.hpp"

#include <memory>
#include <mutex>
#include <vector>

namespace Analyser::Dynamic {

/*!
	Provides a class that multiplexes calls to and from Outputs::Speaker::Speaker in order
	transparently to connect a single caller to multiple destinations.

	Makes a static internal copy of the list of machines; expects the owner to keep it
	abreast of the current frontmost machine.
*/
class MultiSpeaker: public Outputs::Speaker::Speaker, Outputs::Speaker::Speaker::Delegate {
	public:
		/*!
			Provides a construction mechanism that may return nullptr, in the case that all included
			machines return nullptr as their speaker.
		*/
		static MultiSpeaker *create(const std::vector<std::unique_ptr<::Machine::DynamicMachine>> &machines);

		/// This class requires the caller to nominate changes in the frontmost machine.
		void set_new_front_machine(::Machine::DynamicMachine *machine);

		// Below is the standard Outputs::Speaker::Speaker interface; see there for documentation.
		float get_ideal_clock_rate_in_range(float minimum, float maximum) override;
		void set_computed_output_rate(float cycles_per_second, int buffer_size, bool stereo) override;
		bool get_is_stereo() override;
		void set_output_volume(float) override;

	private:
		void speaker_did_complete_samples(Speaker *speaker, const std::vector<int16_t> &buffer) final;
		void speaker_did_change_input_clock(Speaker *speaker) final;
		MultiSpeaker(const std::vector<Outputs::Speaker::Speaker *> &speakers);

		std::vector<Outputs::Speaker::Speaker *> speakers_;
		Outputs::Speaker::Speaker *front_speaker_ = nullptr;
		std::mutex front_speaker_mutex_;

		bool stereo_output_ = false;
};

}
        run: //
//  MultiMachine.cpp
//  Clock Signal
//
//  
//  
//

#include "MultiMachine.hpp"
#include "../../../Outputs/Log.hpp"

#include <algorithm>

namespace {

Log::Logger<Log::Source::MultiMachine> logger;

}

using namespace Analyser::Dynamic;

MultiMachine::MultiMachine(std::vector<std::unique_ptr<DynamicMachine>> &&machines) :
	machines_(std::move(machines)),
	configurable_(machines_),
	timed_machine_(machines_, machines_mutex_),
	scan_producer_(machines_, machines_mutex_),
	audio_producer_(machines_, machines_mutex_),
	joystick_machine_(machines_),
	keyboard_machine_(machines_),
	media_target_(machines_) {
	timed_machine_.set_delegate(this);
}

Activity::Source *MultiMachine::activity_source() {
	return nullptr; // TODO
}

#define Provider(type, name, member)	\
	type *MultiMachine::name() {	\
		if(has_picked_) {	\
			return machines_.front()->name();	\
		} else {	\
			return &member;	\
		}	\
	}

Provider(Configurable::Device, configurable_device, configurable_)
Provider(MachineTypes::TimedMachine, timed_machine, timed_machine_)
Provider(MachineTypes::ScanProducer, scan_producer, scan_producer_)
Provider(MachineTypes::AudioProducer, audio_producer, audio_producer_)
Provider(MachineTypes::JoystickMachine, joystick_machine, joystick_machine_)
Provider(MachineTypes::KeyboardMachine, keyboard_machine, keyboard_machine_)
Provider(MachineTypes::MediaTarget, media_target, media_target_)

MachineTypes::MouseMachine *MultiMachine::mouse_machine() {
	// TODO.
	return nullptr;
}

#undef Provider

bool MultiMachine::would_collapse(const std::vector<std::unique_ptr<DynamicMachine>> &machines) {
	return
		(machines.front()->timed_machine()->get_confidence() > 0.9f) ||
		(machines.front()->timed_machine()->get_confidence() >= 2.0f * machines[1]->timed_machine()->get_confidence());
}

void MultiMachine::did_run_machines(MultiTimedMachine *) {
	std::lock_guard machines_lock(machines_mutex_);

	if constexpr (logger.enabled) {
		auto line = logger.info();
		for(const auto &machine: machines_) {
			auto timed_machine = machine->timed_machine();
			line.append("%0.4f %s; ", timed_machine->get_confidence(), timed_machine->debug_type().c_str());
		}
	}

	DynamicMachine *front = machines_.front().get();
	std::stable_sort(machines_.begin(), machines_.end(),
		[] (const std::unique_ptr<DynamicMachine> &lhs, const std::unique_ptr<DynamicMachine> &rhs){
			auto lhs_timed = lhs->timed_machine();
			auto rhs_timed = rhs->timed_machine();
			return lhs_timed->get_confidence() > rhs_timed->get_confidence();
		});

	if(machines_.front().get() != front) {
		scan_producer_.did_change_machine_order();
		audio_producer_.did_change_machine_order();
	}

	if(would_collapse(machines_)) {
		pick_first();
	}
}

void MultiMachine::pick_first() {
	has_picked_ = true;

	// Ensure output rate specifics are properly copied; these may be set only once by the owner,
	// but rather than being propagated directly by the MultiSpeaker only the derived computed
	// output rate is propagated. So this ensures that if a new derivation is made, it's made correctly.
	if(machines_[0]->audio_producer()) {
		auto multi_speaker = audio_producer_.get_speaker();
		auto specific_speaker = machines_[0]->audio_producer()->get_speaker();

		if(specific_speaker && multi_speaker) {
			specific_speaker->copy_output_rate(*multi_speaker);
		}
	}

	// TODO: because it is not invalid for a caller to keep a reference to anything previously returned,
	// this erase can be added only once the Multi machines that take static copies of the machines list
	// are updated.
	//
	// Example failing use case otherwise: a caller still has reference to the MultiJoystickMachine, and
	// it has dangling references to the various JoystickMachines.
	//
	// This gets into particularly long grass with the MultiConfigurable and its MultiStruct.
//	machines_.erase(machines_.begin() + 1, machines_.end());
}

void *MultiMachine::raw_pointer() {
	return nullptr;
} ./run_your_api.sh & # <- ✏️ update this

      - name: //
//  MultiMachine.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "../../../Machines/DynamicMachine.hpp"

#include "Implementation/MultiProducer.hpp"
#include "Implementation/MultiConfigurable.hpp"
#include "Implementation/MultiProducer.hpp"
#include "Implementation/MultiJoystickMachine.hpp"
#include "Implementation/MultiKeyboardMachine.hpp"
#include "Implementation/MultiMediaTarget.hpp"

#include <memory>
#include <mutex>
#include <vector>

namespace Analyser::Dynamic {

/*!
	Provides the same interface as to a single machine, while multiplexing all
	underlying calls to an array of real dynamic machines.

	Calls to crt_machine->get_crt will return that for the frontmost machine;
	anything installed as the speaker's delegate will similarly receive
	feedback only from that machine.

	Following each crt_machine->run_for, reorders the supplied machines by
	confidence.

	If confidence for any machine becomes disproportionately low compared to
	the others in the set, that machine stops running.
*/
class MultiMachine: public ::Machine::DynamicMachine, public MultiTimedMachine::Delegate {
	public:
		/*!
			Allows a potential MultiMachine creator to enquire as to whether there's any benefit in
			requesting this class as a proxy.

			@returns @c true if the multimachine would discard all but the first machine in this list;
				@c false otherwise.
		*/
		static bool would_collapse(const std::vector<std::unique_ptr<DynamicMachine>> &machines);
		MultiMachine(std::vector<std::unique_ptr<DynamicMachine>> &&machines);

		Activity::Source *activity_source() final;
		Configurable::Device *configurable_device() final;
		MachineTypes::TimedMachine *timed_machine() final;
		MachineTypes::ScanProducer *scan_producer() final;
		MachineTypes::AudioProducer *audio_producer() final;
		MachineTypes::JoystickMachine *joystick_machine() final;
		MachineTypes::KeyboardMachine *keyboard_machine() final;
		MachineTypes::MouseMachine *mouse_machine() final;
		MachineTypes::MediaTarget *media_target() final;
		void *raw_pointer() final;

	private:
		void did_run_machines(MultiTimedMachine *) final;

		std::vector<std::unique_ptr<DynamicMachine>> machines_;
		std::recursive_mutex machines_mutex_;

		MultiConfigurable configurable_;
		MultiTimedMachine timed_machine_;
		MultiScanProducer scan_producer_;
		MultiAudioProducer audio_producer_;
		MultiJoystickMachine joystick_machine_;
		MultiKeyboardMachine keyboard_machine_;
		MultiMediaTarget media_target_;

		void pick_first();
		bool has_picked_ = false;
};

}
        uses: //
//  ConfidenceCounter.cpp
//  Clock Signal
//
//  
//  
//

#include "ConfidenceCounter.hpp"

using namespace Analyser::Dynamic;

float ConfidenceCounter::get_confidence() {
	return float(hits_) / float(hits_ + misses_);
}

void ConfidenceCounter::add_hit() {
	++hits_;
}

void ConfidenceCounter::add_miss() {
	++misses_;
}

void ConfidenceCounter::add_equivocal() {
	if(hits_ > misses_) {
		++hits_;
		++misses_;
	}
}
        continue-on-error: //
//  ConfidenceCounter.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "ConfidenceSource.hpp"

namespace Analyser::Dynamic {

/*!
	Provides a confidence source that calculates its probability by virtual of a history of events.

	The initial value of the confidence counter is 0.5.
*/
class ConfidenceCounter: public ConfidenceSource {
	public:
		/*! @returns The computed probability, based on the history of events. */
		float get_confidence() final;

		/*! Records an event that implies this is the appropriate class: pushes probability up towards 1.0. */
		void add_hit();

		/*! Records an event that implies this is not the appropriate class: pushes probability down towards 0.0. */
		void add_miss();

		/*!
			Records an event that could be correct but isn't necessarily so; which can push probability
			down towards 0.5, but will never push it upwards.
		*/
		void add_equivocal();

	private:
		int hits_ = 1;
		int misses_ = 1;
};

}
        with: //
//  ConfidenceSource.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

namespace Analyser::Dynamic {

/*!
	Provides an abstract interface through which objects can declare the probability
	that they are the proper target for their input; e.g. if an Acorn Electron is asked
	to run an Atari 2600 program then its confidence should shrink towards 0.0; if the
	program is handed to an Atari 2600 then its confidence should grow towards 1.0.
*/
struct ConfidenceSource {
	virtual float get_confidence() = 0;
};

}
          mayhem-token: ${{ //
//  ConfidenceSummary.cpp
//  Clock Signal
//
//  
//  
//

#include "ConfidenceSummary.hpp"

#include <cassert>
#include <numeric>

using namespace Analyser::Dynamic;

ConfidenceSummary::ConfidenceSummary(const std::vector<ConfidenceSource *> &sources, const std::vector<float> &weights) :
	sources_(sources), weights_(weights) {
	assert(weights.size() == sources.size());
	weight_sum_ = std::accumulate(weights.begin(), weights.end(), 0.0f);
}

float ConfidenceSummary::get_confidence() {
	float result = 0.0f;
	for(std::size_t index = 0; index < sources_.size(); ++index) {
		result += sources_[index]->get_confidence() * weights_[index];
	}
	return result / weight_sum_;
} }}
          api-url: //
//  ConfidenceSummary.hpp
//  Clock Signal
//
//  
//  
//

#pragma once

#include "ConfidenceSource.hpp"

#include <vector>

namespace Analyser::Dynamic {

/*!
	Summaries a collection of confidence sources by calculating their weighted sum.
*/
class ConfidenceSummary: public ConfidenceSource {
	public:
		/*!
			Instantiates a summary that will produce the weighted sum of
			@c sources, each using the corresponding entry of @c weights.

			Requires that @c sources and @c weights are of the same length.
		*/
		ConfidenceSummary(
			const std::vector<ConfidenceSource *> &sources,
			const std::vector<float> &weights);

		/*! @returns The weighted sum of all sources. */
		float get_confidence() final;

	private:
		const std::vector<ConfidenceSource *> sources_;
		const std::vector<float> weights_;
		float weight_sum_;
};

} http://localhost:8080 # <- ✏️ update this
          api-spec: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use GuzzleHttp\Psr7\Message;
use GuzzleHttp\Psr7\Response;

trait CloseResponseTrait {
    /**
     * Close a connection with an HTTP response
     * @param \Ratchet\ConnectionInterface $conn
     * @param int                          $code HTTP status code
     * @return null
     */
    private function close(ConnectionInterface $conn, $code = 400, array $additional_headers = []) {
        $response = new Response($code, array_merge([
            'X-Powered-By' => \Ratchet\VERSION
        ], $additional_headers));

        $conn->send(Message::toString($response));
        $conn->close();
    }
} http://localhost:8080/openapi.json # <- ✏️ update this
          duration: <?php
namespace Ratchet\Http;
use Ratchet\MessageInterface;
use Ratchet\ConnectionInterface;
use GuzzleHttp\Psr7\Message;

/**
 * This class receives streaming data from a client request
 * and parses HTTP headers, returning a PSR-7 Request object
 * once it's been buffered
 */
class HttpRequestParser implements MessageInterface {
    const EOM = "\r\n\r\n";

    /**
     * The maximum number of bytes the request can be
     * This is a security measure to prevent attacks
     * @var int
     */
    public $maxSize = 4096;

    /**
     * @param \Ratchet\ConnectionInterface $context
     * @param string                       $data Data stream to buffer
     * @return \Psr\Http\Message\RequestInterface
     * @throws \OverflowException If the message buffer has become too large
     */
    public function onMessage(ConnectionInterface $context, $data) {
        if (!isset($context->httpBuffer)) {
            $context->httpBuffer = '';
        }

        $context->httpBuffer .= $data;

        if (strlen($context->httpBuffer) > (int)$this->maxSize) {
            throw new \OverflowException("Maximum buffer size of {$this->maxSize} exceeded parsing HTTP header");
        }

        if ($this->isEom($context->httpBuffer)) {
            $request = $this->parse($context->httpBuffer);

            unset($context->httpBuffer);

            return $request;
        }
    }

    /**
     * Determine if the message has been buffered as per the HTTP specification
     * @param  string  $message
     * @return boolean
     */
    public function isEom($message) {
        return (boolean)strpos($message, static::EOM);
    }

    /**
     * @param string $headers
     * @return \Psr\Http\Message\RequestInterface
     */
    public function parse($headers) {
        return Message::parseRequest($headers);
    }
}
          sarif-report: <?php
namespace Ratchet\Http;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class HttpServer implements MessageComponentInterface {
    use CloseResponseTrait;

    /**
     * Buffers incoming HTTP requests returning a Guzzle Request when coalesced
     * @var HttpRequestParser
     * @note May not expose this in the future, may do through facade methods
     */
    protected $_reqParser;

    /**
     * @var \Ratchet\Http\HttpServerInterface
     */
    protected $_httpServer;

    /**
     * @param HttpServerInterface
     */
    public function __construct(HttpServerInterface $component) {
        $this->_httpServer = $component;
        $this->_reqParser  = new HttpRequestParser;
    }

    /**
     * {@inheritdoc}
     */
    public function onOpen(ConnectionInterface $conn) {
        $conn->httpHeadersReceived = false;
    }

    /**
     * {@inheritdoc}
     */
    public function onMessage(ConnectionInterface $from, $msg) {
        if (true !== $from->httpHeadersReceived) {
            try {
                if (null === ($request = $this->_reqParser->onMessage($from, $msg))) {
                    return;
                }
            } catch (\OverflowException $oe) {
                return $this->close($from, 413);
            }

            $from->httpHeadersReceived = true;

            return $this->_httpServer->onOpen($from, $request);
        }

        $this->_httpServer->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    public function onClose(ConnectionInterface $conn) {
        if ($conn->httpHeadersReceived) {
            $this->_httpServer->onClose($conn);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function onError(ConnectionInterface $conn, \Exception $e) {
        if ($conn->httpHeadersReceived) {
            $this->_httpServer->onError($conn, $e);
        } else {
            $this->close($conn, 500);
        }
    }
}

      - name: <?php
namespace Ratchet\Http;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;
use Psr\Http\Message\RequestInterface;

interface HttpServerInterface extends MessageComponentInterface {
    /**
     * @param \Ratchet\ConnectionInterface          $conn
     * @param \Psr\Http\Message\RequestInterface    $request null is default because PHP won't let me overload; don't pass null!!!
     * @throws \UnexpectedValueException if a RequestInterface is not passed
     */
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null);
}
        uses: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use Psr\Http\Message\RequestInterface;

class NoOpHttpServerController implements HttpServerInterface {
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null) {
    }

    public function onMessage(ConnectionInterface $from, $msg) {
    }

    public function onClose(ConnectionInterface $conn) {
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
    }
}
        with: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use Ratchet\MessageComponentInterface;
use Psr\Http\Message\RequestInterface;

/**
 * A middleware to ensure JavaScript clients connecting are from the expected domain.
 * This protects other websites from open WebSocket connections to your application.
 * Note: This can be spoofed from non-web browser clients
 */
class OriginCheck implements HttpServerInterface {
    use CloseResponseTrait;

    /**
     * @var \Ratchet\MessageComponentInterface
     */
    protected $_component;

    public $allowedOrigins = [];

    /**
     * @param MessageComponentInterface $component Component/Application to decorate
     * @param array                     $allowed   An array of allowed domains that are allowed to connect from
     */
    public function __construct(MessageComponentInterface $component, array $allowed = []) {
        $this->_component = $component;
        $this->allowedOrigins += $allowed;
    }

    /**
     * {@inheritdoc}
     */
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null) {
        $header = (string)$request->getHeader('Origin')[0];
        $origin = parse_url($header, PHP_URL_HOST) ?: $header;

        if (!in_array($origin, $this->allowedOrigins)) {
            return $this->close($conn, 403);
        }

        return $this->_component->onOpen($conn, $request);
    }

    /**
     * {@inheritdoc}
     */
    function onMessage(ConnectionInterface $from, $msg) {
        return $this->_component->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    function onClose(ConnectionInterface $conn) {
        return $this->_component->onClose($conn);
    }

    /**
     * {@inheritdoc}
     */
    function onError(ConnectionInterface $conn, \Exception $e) {
        return $this->_component->onError($conn, $e);
    }
}
          sarif_file: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use Psr\Http\Message\RequestInterface;
use Symfony\Component\Routing\Matcher\UrlMatcherInterface;
use Symfony\Component\Routing\Exception\MethodNotAllowedException;
use Symfony\Component\Routing\Exception\ResourceNotFoundException;
use GuzzleHttp\Psr7\Query;

class Router implements HttpServerInterface {
    use CloseResponseTrait;

    /**
     * @var \Symfony\Component\Routing\Matcher\UrlMatcherInterface
     */
    protected $_matcher;

    private $_noopController;

    public function __construct(UrlMatcherInterface $matcher) {
        $this->_matcher = $matcher;
        $this->_noopController = new NoOpHttpServerController;
    }

    /**
     * {@inheritdoc}
     * @throws \UnexpectedValueException If a controller is not \Ratchet\Http\HttpServerInterface
     */
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null) {
        if (null === $request) {
            throw new \UnexpectedValueException('$request can not be null');
        }

        $conn->controller = $this->_noopController;

        $uri = $request->getUri();

        $context = $this->_matcher->getContext();
        $context->setMethod($request->getMethod());
        $context->setHost($uri->getHost());

        try {
            $route = $this->_matcher->match($uri->getPath());
        } catch (MethodNotAllowedException $nae) {
            return $this->close($conn, 405, array('Allow' => $nae->getAllowedMethods()));
        } catch (ResourceNotFoundException $nfe) {
            return $this->close($conn, 404);
        }

        if (is_string($route['_controller']) && class_exists($route['_controller'])) {
            $route['_controller'] = new $route['_controller'];
        }

        if (!($route['_controller'] instanceof HttpServerInterface)) {
            throw new \UnexpectedValueException('All routes must implement Ratchet\Http\HttpServerInterface');
        }

        $parameters = [];
        foreach($route as $key => $value) {
            if ((is_string($key)) && ('_' !== substr($key, 0, 1))) {
                $parameters[$key] = $value;
            }
        }
        $parameters = array_merge($parameters, Query::parse($uri->getQuery() ?: ''));

        $request = $request->withUri($uri->withQuery(Query::build($parameters)));

        $conn->controller = $route['_controller'];
        $conn->controller->onOpen($conn, $request);
    }

    /**
     * {@inheritdoc}
     */
    public function onMessage(ConnectionInterface $from, $msg) {
        $from->controller->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    public function onClose(ConnectionInterface $conn) {
        if (isset($conn->controller)) {
            $conn->controller->onClose($conn);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function onError(ConnectionInterface $conn, \Exception $e) {
        if (isset($conn->controller)) {
            $conn->controller->onError($conn, $e);
        }
    }
}
