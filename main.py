from vpython import *
import random


class Led:
  '''A single LED'''

  def __init__(self, x, y, z):
    self.sphere = sphere(pos=vector(x, y, z), radius=0.1)

  def color(self, c):
    '''Set the color'''
    self.sphere.color = c


class LedRow:
  '''A row of LEDs'''

  def __init__(self, x, y):
    self.leds = []
    for z in range(0, 10):
      self.leds.append(Led(x, y, z))

  def grad(self, start, end):
    '''Apply a linear gradient to the row'''
    for i, led in enumerate(self.leds):
      c = start + i * (end-start)/len(self.leds)
      led.color(c)

  def pulse(self, index, base_color, pulse_color, width=5):
    '''A triangular intensity pulse'''
    r = 0.5 * width
    for i, led in enumerate(self.leds):
      k = max(0, 1 - abs(i-index)/r)
      c = base_color + k * (pulse_color-base_color)
      led.color(c)

  def off(self):
    for led in self.leds:
      led.color(color.black)

def random_color():
  '''Generate a random color'''
  c = color.white
  c.x = random.random()
  c.y = random.random()
  c.z = random.random()
  return c


class LedTunnel:
  '''A tunnel of LED rows'''

  def __init__(self):
    self.a = LedRow(-0.866, -0.5)
    self.b = LedRow(-0.866, 0.5)
    self.c = LedRow(0, 1)
    self.d = LedRow(0.866, 0.5)
    self.e = LedRow(0.866, -0.5)

  def rows(self):
    return self.a, self.b, self.c, self.d, self.e

  def linear_gradients(self):
    self.a.grad(color.yellow, color.red)
    self.b.grad(color.blue, color.purple)
    self.c.grad(color.green, color.yellow)
    self.d.grad(color.purple, color.cyan)
    self.e.grad(color.orange, color.magenta)

  def cyan_pulses(self):
    for i in range(0, 1000):
      rate(50)
      self.a.pulse((0+i/10) % 10, color.black, color.cyan)
      self.b.pulse((6+i/10) % 10, color.black, color.cyan)
      self.c.pulse((2+i/10) % 10, color.black, color.cyan)
      self.d.pulse((8+i/10) % 10, color.black, color.cyan)
      self.e.pulse((4+i/10) % 10, color.black, color.cyan)

  def disco_party(self):
    for _ in range(0, 1000):
      rate(5)
      i = random.randint(0, 4)
      for j, x in enumerate(self.rows()):
        if i == j:
          k = random.random() * 15 - 2.5
          x.pulse(k, color.black, random_color())
        else:
          x.off()


def main():
  t = LedTunnel()

  # linear_gradients()
  t.cyan_pulses()
  # disco_party()


if __name__ == '__main__':
    main()
