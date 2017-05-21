from enum import Enum

import time

import math

from behavior import Behavior

FUNNY_ACTION_TIME = 90  # in seconds
END_MATCH_TIME = 95  # in seconds

#2017 specific
SMALL_CRATER_COLLECT_DURATION = 8 # in seconds
SMALL_CRATER_FIRE_DURATION = 7 # in seconds

class Color(Enum):
    BLUE = "blue"
    YELLOW = "yellow"


class FSMMatch(Behavior):
    def __init__(self, robot):
        self.robot = robot
        self.color = None
        self.start_time = None
        self.state = StateColorSelection(self)

    def loop(self):
        time_now = time.time()
        if self.start_time is not None and time_now - self.start_time >= FUNNY_ACTION_TIME:  # Checks time for funny action!
            next_state = StateFunnyAction
        elif self.start_time is not None and time_now - self.start_time >= END_MATCH_TIME:
            next_state = StateEnd
        else:
            next_state = self.state.test()
        if next_state is not None:
            self.state.deinit()
            self.state = next_state(self)

    def start_match(self):
        self.start_time = time.time()



class FSMState:
    def __init__(self, behavior):
        raise NotImplementedError("this state is not defined yet")

    def test(self):
        raise NotImplementedError("test of this state is not defined yet !")

    def deinit(self):
        raise NotImplementedError("deinit of this state is not defined yet !")


class StateColorSelection(FSMState):
    class ColorState(Enum):
        IDLE = "idle"
        PRESSED = "pressed"

    def __init__(self, behavior):
        self.behavior = behavior
        self.behavior.color = Color.BLUE
        self.state = self.ColorState.IDLE
        self.behavior.robot.io.set_led_color(self.behavior.robot.io.LedColor.BLUE)

    def test(self):
        if self.state == self.ColorState.IDLE and self.behavior.robot.io.button_state == self.behavior.robot.io.ButtonState.PRESSED:
            if self.behavior.color == Color.YELLOW:
                self.behavior.robot.io.set_led_color(self.behavior.robot.io.LedColor.BLUE)
                self.behavior.color = Color.BLUE
            else:
                self.behavior.robot.io.set_led_color(self.behavior.robot.io.LedColor.YELLOW)
                self.behavior.color = Color.YELLOW
            self.state = self.ColorState.PRESSED

        if self.state == self.ColorState.PRESSED and self.behavior.robot.io.button_state == self.behavior.robot.io.ButtonState.RELEASED:
            self.state = self.ColorState.IDLE

        if self.behavior.robot.io.cord_state == self.behavior.robot.io.CordState.OUT:
            if self.behavior.color == Color.YELLOW:
                return StateTraj1Yellow
            else:
                return StateTraj1Blue

    def deinit(self):
        self.behavior.start_match()
        if self.behavior.color == Color.YELLOW:
            self.behavior.robot.locomotion.reposition_robot(2955, 1800, math.pi)
        else:
            self.behavior.robot.locomotion.reposition_robot(200, 1800, 0)





class StateTraj1Yellow(FSMState):
    def __init__(self, behavior):
        self.behavior = behavior
        self.stopped = False
        p1 = self.behavior.robot.locomotion.Point(2150, 1820)
        p2 = self.behavior.robot.locomotion.Point(1850, 1700)
        p3 = self.behavior.robot.locomotion.Point(1980, 1450)
        self.behavior.robot.locomotion.follow_trajectory([p1, p2, p3], theta=0, speed=100)

    def test(self):
        if self.behavior.robot.io.front_distance <= 15 and not self.stopped:
            self.behavior.robot.locomotion.stop_robot()
            self.stopped = True
        if self.behavior.robot.io.front_distance > 15 and self.stopped:
            self.behavior.robot.locomotion.restart_robot()
            self.stopped = False

        if self.behavior.robot.locomotion.is_trajectory_finished:
            return StateSmallCrater1Yellow

    def deinit(self):
        pass


class StateTraj1Blue(FSMState):
    def __init__(self, behavior):
        self.behavior = behavior
        pass

    def test(self):
        pass

    def deinit(self):
        pass

class StateSmallCrater1Yellow(FSMState):
    def __init__(self, behavior):
        self.behavior = behavior
        self.stopped = False
        self.behavior.robot.locomotion.go_to_orient(2250, 1450, 0, 50)
        self.behavior.robot.io.start_ball_picker()
        self.ball_picker_start_time = time.time()

    def test(self):
        if self.behavior.robot.io.front_distance <= 15 and not self.stopped:
            self.behavior.robot.locomotion.stop_robot()
            self.stopped = True
        if self.behavior.robot.io.front_distance > 15 and self.stopped:
            self.behavior.robot.locomotion.restart_robot()
            self.stopped = False

        collect_time = time.time() - self.ball_picker_start_time
        if self.behavior.robot.locomotion.is_trajectory_finished and collect_time >= SMALL_CRATER_COLLECT_DURATION:
            return StateTrajFirePositionYellow1

    def deinit(self):
        pass


class StateTrajFirePositionYellow1(FSMState):
    def __init__(self, behavior):
        self.behavior = behavior
        self.stopped = False
        self.behavior.robot.locomotion.go_to_orient(2600, 1300, 4.50, 100)
        self.fire_started = False
        self.fire_start_time = 0

    def test(self):
        if self.behavior.robot.io.front_distance <= 15 and not self.stopped:
            self.behavior.robot.locomotion.stop_robot()
            self.stopped = True
        if self.behavior.robot.io.front_distance > 15 and self.stopped:
            self.behavior.robot.locomotion.restart_robot()
            self.stopped = False

        if self.behavior.robot.locomotion.is_trajectory_finished and not self.fire_started:
            self.fire_start_time = time.time()
            self.behavior.robot.io.open_cannon_barrier()
            self.behavior.robot.io.start_cannon()

        if  self.fire_started:
            fire_duration = time.time() - self.fire_start_time
            if fire_duration > SMALL_CRATER_FIRE_DURATION:
                self.behavior.robot.io.stop_cannon()
                return StateEnd

    def deinit(self):
        pass



class StateFunnyAction(FSMState):
    def __init__(self, behavior):
        self.behavior = behavior
        self.behavior.robot.locomotion.stop_robot()
        self.behavior.robot.io.open_rocket_launcher()

    def test(self):
        return StateEnd

    def deinit(self):
        pass


class StateEnd(FSMState):
    def __init__(self, behavior):
        self.behavior = behavior
        self.behavior.robot.locomotion.stop_robot()

    def test(self):
        pass

    def deinit(self):
        pass
