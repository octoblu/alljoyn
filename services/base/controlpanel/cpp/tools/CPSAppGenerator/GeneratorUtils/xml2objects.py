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
from xml.sax.handler import ContentHandler
import xml.sax
import sys

DEBUG = False

class Node:

	def __init__(self, name, stack=None):
		self._name = name
		self.wow = ''
		self.nxt = 'wow'
		self.stack = stack
		self.parked_attrs = dict()
		self.attr = dict()
		self.sub_nodes = []
		if self.stack == None:
			self.stack = [self]

	def _get_next(self):
		a = getattr(self, self.nxt)
		if isinstance(a, list):
			if len(a): return a[-1]
			else: return 'wow'
		else:
			return a

	def _set_next(self, val):
		if hasattr(self, self.nxt):
			a = getattr(self, self.nxt)
			if isinstance(a, list):
				if len(a) and a[-1] == 'in limbo':
					a[-1] = val
				else:
					a.append(val)
				return
		setattr(self, self.nxt, val)

	def _clear_next(self):
		self.nxt = 'wow'

	def _park_attrs(self, attrs):
		if not attrs.getLength(): return
		for n in attrs.getNames():
			self.parked_attrs[n] = attrs.getValue(n)
		self.debug('>Parking attrs ' + str(self.parked_attrs))

	def _unpark_attrs(self, onto):
		if not len(self.parked_attrs): return
		onto.attr = self.parked_attrs
		self.parked_attrs = dict()
		self.debug('>Unparking attrs onto ' + str(onto))

	def __str__(self):
		return '%s:%s' % (type(self), self._name)

	def debug(self, s):
		if not DEBUG: return
		print '%s%s [%s]' % (' ' * len(self.stack), s, self._name)

	def element(self, name, attrs):
		self.debug('Starting element ' + name)

		if hasattr(self, name):
			a = getattr(self, name)
			if isinstance(a, list):
				pass
			elif isinstance(a, (basestring, Node)) and str(self._get_next()) != 'in limbo':
				self.debug('> moving %s to list' % name)
				l = list()
				l.append(a)
				setattr(self, name, l)

		if self._get_next() == 'in limbo':

			n = Node(self.nxt, self.stack)
			self.sub_nodes.append(n)
			self.debug('>Moving out of limbo ' + str(n))
			self._unpark_attrs(n)
			self._set_next(n)
			self.stack.pop()
			self.stack.append(n)

			self._clear_next()
			n._park_attrs(attrs)
			n.add_member(name, attrs)
		else:
			self._park_attrs(attrs)
			self.add_member(name, attrs)
			
	def add_member(self, name, attrs):
		self.debug('> %s in limbo' % name)
		self.nxt = name
		self._set_next('in limbo')
		self.stack.append(self)

	def characters(self, content):
		c = content.strip()
		if '' == c: return
		s = self._get_next()
		if s == 'in limbo': 
			self.debug('> moving %s to string' % self.nxt)
			s = type('attredstr', (type(content),), {})()
		s = type(s)(s + c)
		self.debug('> setting value to %s' % str(s))
		self._unpark_attrs(s)
		self._set_next(s)


	def end_element(self):
		self.stack.pop()
		self._clear_next()

	def _fields(self):
		blank = Node('')
		return filter(lambda x: x not in dir(blank), dir(self))

	def __len__(self):
		return 1

	def __getitem__(self, index):
		if index != 0: raise IndexError()
		else: return self

	def _dump_text_field(self, field, val, tab):
		print '%s%s=%s' % (tab, field, val)
		if not hasattr(val, 'attr'): return

		for k,v in val.attr.items():
			print ' %s%s.%s=%s' % (tab, field, k, v)


	def _dump(self, indent=0):
		tab = ' ' * indent
		for k, v in self.attr.items():
			print '%s%s.%s=%s' % (tab, self._name, k, v)

		for field in filter(lambda x: isinstance(getattr(self, x), basestring), self._fields()):
			fieldval = getattr(self, field)
			self._dump_text_field(field, fieldval, tab)
			
		for field in filter(lambda x: isinstance(getattr(self, x), Node), self._fields()):
			print '%s%s' % (tab, field)
			getattr(self, field)._dump(indent + 1)

		for field in filter(lambda x: isinstance(getattr(self, x), list), self._fields()):
			counter = 0
			for entry in getattr(self, field):
				if isinstance(entry, basestring):
					self._dump_text_field('%s[%i]' % (field, counter), entry, tab)
				else:
					print '%s%s' % (tab, field)
					entry._dump(indent + 1)
				counter += 1

	def _find_field(self, field_name, res = None):
		if res == None: res = list()

		if hasattr(self, field_name):
			o = getattr(self, field_name)
			if isinstance(o, list):
				res.extend(o)
			else:
				res.append(o)

		for field in self._fields():
			o = getattr(self, field)
			if isinstance(o, Node): 
				o._find_field(field_name, res)
			elif isinstance(o, list) and len(o) and isinstance(o[0], Node):
				for i in o: i._find_field(field_name, res)

		return res	

	def _has(self, field_name):
		return hasattr(self, field_name)

	def _kids(self):
		return map(lambda x: getattr(self, x), self._fields())
						

class ObjectBuilder(ContentHandler):

	class Root(Node):
		pass

	def __init__(self, path):
		parser = xml.sax.make_parser()
		parser.setContentHandler(self)
		self.root = ObjectBuilder.Root('magic root')
		parser.parse(open(path,'r'))

	def top(self):
		return self.root.stack[-1]

	def startElement(self, name, attrs):
		self.top().element(name, attrs)

	def characters(self, content):
		self.top().characters(content)

	def endElement(self, name):
		self.top().end_element()

if __name__ == '__main__':
	o = ObjectBuilder('SampleXMLs/testListProperty.xml')
#o.root._dump()
	o.root._dump()

