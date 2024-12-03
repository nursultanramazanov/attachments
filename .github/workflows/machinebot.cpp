# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: //
//  ActivityObserver.h
//  Clock Signal
//
//  Created by on 07/05/2018.
//  Copyright 2018. All rights reserved.
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
//  Created by on 07/05/2018.
//  Copyright 2018 . All rights reserved.
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
//  Created by on 09/02/2018.
//  Copyright 2018. All rights reserved.
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
    branches: [ "main" ]
  schedule: //
//  MultiConfigurable.hpp
//  Clock Signal
//
//  Created by on 09/02/2018.
//  Copyright 2018. All rights reserved.
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
    - cron: //
//  MultiJoystickMachine.cpp
//  Clock Signal
//
//  Created by on 09/02/2018.
//  Copyright 2018. All rights reserved.
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

env: //
//  MultiJoystickMachine.hpp
//  Clock Signal
//
//  Created by on 09/02/2018.
//  Copyright 2018. All rights reserved.
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
  # Path to the CMake build directory.
  build: //
//  MultiKeyboardMachine.cpp
//  Clock Signal
//
//  Created by on 09/02/2018.
//  Copyright 2018 . All rights reserved.
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
//  Created by on 09/02/2018.
//  Copyright 2018. All rights reserved.
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
  contents: //
//  MultiMediaTarget.cpp
//  Clock Signal
//
//  Created by on 29/01/2018.
//  Copyright 2018. All rights reserved.
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

jobs: //
//  MultiMediaTarget.hpp
//  Clock Signal
//
//  Created by on 29/01/2018.
//  Copyright 2018. All rights reserved.
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
  analyze: //
//  MultiProducer.cpp
//  Clock Signal
//
//  Created by on 29/01/2018.
//  Copyright 2018. All rights reserved.
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
    permissions: //
//  MultiProducer.hpp
//  Clock Signal
//
//  Created by on 29/01/2018.
//  Copyright 2018. All rights reserved.
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
      contents: //
//  MultiSpeaker.cpp
//  Clock Signal
//
//  Created by on 18/02/2018.
//  Copyright 2018. All rights reserved.
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
}   # for actions/checkout to fetch code
      security-events: //
//  MultiSpeaker.hpp
//  Clock Signal
//
//  Created by on 18/02/2018.
//  Copyright 2018. All rights reserved.
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

} # for github/codeql-action/upload-sarif to upload SARIF results
      actions: cp vendor/components/jquery/jquery.min.js web/vendor-js/jquery.min.js

cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css web/vendor-css/bootstrap.min.css
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js web/vendor-js/bootstrap.min.js

cp vendor/twbs/bootstrap/dist/fonts/* web/fonts/

cp -r vendor/ckeditor/ckeditor/adapters web/ckeditor/adapters
cp -r vendor/ckeditor/ckeditor/lang web/ckeditor/lang
cp -r vendor/ckeditor/ckeditor/plugins web/ckeditor/plugins
cp -r vendor/ckeditor/ckeditor/skins web/ckeditor/skins
cp vendor/ckeditor/ckeditor/ckeditor.js web/ckeditor/ckeditor.js
cp vendor/ckeditor/ckeditor/config.js web/ckeditor/config.js
cp vendor/ckeditor/ckeditor/contents.css web/ckeditor/contents.css
cp vendor/ckeditor/ckeditor/styles.js web/ckeditor/styles.js
# only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ env.build }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with:
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
