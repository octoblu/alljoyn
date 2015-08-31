# Copyright AllSeen Alliance. All rights reserved.
#
#    Permission to use, copy, modify, and/or distribute this software for any
#    purpose with or without fee is hereby granted, provided that the above
#    copyright notice and this permission notice appear in all copies.
#
#    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
import xml2objects
from sys import stderr

class Validator():

	def validate(self, root):
		ret = True
		for obj in root._find_field(self.__class__.__name__):
			self.root = obj
			for rule in filter(lambda x: x.startswith('rule_'), dir(self)):
				rule_func = getattr(self, rule)
				if not rule_func():
					stderr.write('Error validating "%s": %s\n' % (self.__class__.__name__, rule_func.__doc__))
					ret = False
		return ret
	
	def check_hints(self, valid_hints):
		for hint in self.root._find_field('hint'):
			if hint not in valid_hints:
				return False
		return True

	def has_hint(self, hint):
		return hint in self.root._find_field('hint')

	def has_constraintVals(self):
		return self.root._find_field('constraintVals')

	def has_constraintRange(self):
		return self.root._find_field('constraintRange')

class scalarProperty(Validator):

	def rule_hints(self):
		'hints must be one of ["spinner", "radiobutton", "slider", "numberpicker", "keypad", "numericview"]'
		return self.check_hints(valid_hints = ["spinner", "radiobutton", "slider", "numberpicker", "keypad", "numericview"])

	def rule_slider_only_range(self):
		'hint of type "slider" must have a contraintRange'
		if not self.has_hint('slider'): return True
		return self.has_constraintRange()

	def rule_numberpicker_has_range(self):
		'a "numberpicker" must have a constraint Range'
		if not self.has_hint('numberpicker'): return True
	 	return self.has_constraintRange()	 

	def rule_keypad_no_constraints(self):
		'keypad must have no constraints'
		if not self.has_hint('keypad'): return True
		return not (self.has_constraintVals() or self.has_constraintRange())

	def rule_numericview_no_constraints(self):
		'numericview must have no constraints'
		if not self.has_hint('numericview'): return True
		return not (self.has_constraintVals() or self.has_constraintRange())

	def rule_spinner_requires_constraint(self):
		'a spinner requires a constraintRange or connstraintVals'
		if not self.has_hint('spinner'): return True
		return (self.has_constraintVals() or self.has_constraintRange())

	def rule_radiobutton_requires_constraint(self):
		'a radiobutton requires a constraintRange or connstraintVals'
		if not self.has_hint('radiobutton'): return True
		return (self.has_constraintVals() or self.has_constraintRange())

class stringProperty(Validator):

	def rule_hints(self):
		'hints must be one of ["switch", "spinner", "radiobutton", "textview", "edittext"]'
		return self.check_hints(valid_hints = ["switch", "spinner", "radiobutton", "textview", "edittext"])

	def rule_no_ranges(self):
		'a string property can not have a constraintRange'
		return not self.has_constraintRange()

	def rule_switch_has_2_hints(self):
		'a switch must have exactly two constraintVals'
		if not self.has_hint('switch'): return True
		constraints = self.root._find_field('constraint')
		return 2 == len(constraints)

	def rule_spinner_requires_constraint(self):
		'a spinner requires connstraintVals'
		if not self.has_hint('spinner'): return True
		return self.has_constraintVals()

	def rule_radiobutton_requires_constraint(self):
		'a radiobutton requires connstraintVals'
		if not self.has_hint('radiobutton'): return True
		return self.has_constraintVals()

	def rule_textview_no_constraints(self):
		'textview must have no constraints'
		if not self.has_hint('textview'): return True
		return not (self.has_constraintVals() or self.has_constraintRange())

	def rule_edittext_no_constraints(self):
		'edittext must have no constraints'
		if not self.has_hint('edittext'): return True
		return not (self.has_constraintVals() or self.has_constraintRange())

class dateProperty(Validator):

	def rule_hints(self):
		'hints must be one of ["datepicker"]'
		return self.check_hints(valid_hints = ["datepicker"])


class timeProperty(Validator):

	def rule_hints(self):
		'hints must be one of ["timepicker"]'
		return self.check_hints(valid_hints = ["timepicker"])


class booleanProperty(Validator):

	def rule_hints(self):
		'hints must be one of ["checkbox"]'
		return self.check_hints(valid_hints = ["checkbox"])


def validate_all(root):
	ret = True
	ret = scalarProperty().validate(root) and ret
	ret = stringProperty().validate(root) and ret
	ret = dateProperty().validate(root) and ret
	ret = timeProperty().validate(root) and ret
	ret = booleanProperty().validate(root) and ret
	return ret

