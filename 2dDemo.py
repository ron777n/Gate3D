import math
import numpy
import cv2
import threading

screen_size = (1000, 600)

class Point:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

    def __str__(self):  # print(object) shows this
        return f"({self.x}, {self.y})"

    def __repr__(self): # shows this in lists
        return str(self)

    def __eq__(self, other): # ==
        return self.x == other.x and self.y == other.y

    def __add__(self, other): # +
        return Point(self.x + other.x, self.y + other.y)

    def __mul__(self, value): # *
        return Point(self.x * value, self.y * value)

    def __abs__(self):
        return Point(abs(self.x), abs(self.y))

    def __iter__(self):
        return iter((self.x, self.y))

    def __neg__(self):
        return Point(-self.x, -self.y)

    def __sub__(self, other):
        return self + -other

    def __round__(self):
        return Point(int(self.x), int(self.y))


class Face:
    vertices: list[Point]

    def __init__(self, *vertices: Point):
        self.vertices = []
        for vertex in vertices:
            self.vertices.append(vertex)

    def __contains__(self, key: Point): # is coordinate in the area the face takes
        return key in self.vertices

    def __add__(self, point: Point):
        return Face(*[v + point for v in self.vertices])

    def __imul__(self, value):
        for i, v in enumerate(self.vertices):
            self.vertices[i] = v * value

    def __str__(self):
        return "face<" + ", ".join([str(v) for v in self.vertices]) + ">"

    def get_lines(self):
        lines = []
        v_count = len(self.vertices)
        for i in range(v_count):
            lines.append([self.vertices[i], self.vertices[(i+1) % v_count]])
        return lines
        # return [(self.vertices[i], self.vertices[(i+1)%4]) for i in range(4)]


class Shape:
    center: Point
    faces: list[Face]
    def __init__(self, center, faces: list[Face]):
        self.center = center
        self.faces = faces.copy()

    def render(self, renderer: 'Renderer'):
        for face in self.faces:
            for line in face.get_lines():
                renderer.draw_line(line[0] + self.center, line[1] + self.center)
    def scale(self, amount):
        for face in self.faces:
            face *= amount

    def extrude(self, face_index, extrusion_vector):
        if isinstance(face_index, tuple):
            old_face = self.faces[face_index[0]]
            line = old_face.get_lines()[face_index[1]]
            face = Face(*line, *[p + extrusion_vector for p in line[::-1]])
        else:
            old_face = self.faces[face_index]
            new_vertices = [vertex + extrusion_vector for vertex in old_face.vertices]
            face = Face(*new_vertices)

        self.faces.append(face)
    

class Renderer:
    size: tuple[int, int]

    def __init__(self, width, height):
        self.size = width, height
        self.midpoint = Point(self.size[0] / 2, self.size[0] / 2)
        self.reset_frame()
        cv2.namedWindow("Box")

    def draw_line(self, point_a, point_b):
        cv2.line(self.frame, tuple(round(point_a + self.midpoint)), tuple(round(point_b + self.midpoint)), (255, 255, 255))


    def render(self):
        cv2.imshow("Box", self.frame)
        self.reset_frame()
        cv2.waitKey(1)

    def reset_frame(self):
        self.frame = numpy.zeros(shape=(self.size[0], self.size[1], 3))
        cv2.circle(self.frame, tuple(round(self.midpoint)), 25, (255, 255, 255))


def box_maker(center=Point()) -> Shape:
    face = Face(*[Point(*i) for i in ((-5, -5), (-5, 5), (5, 5), (5, -5))])
    return Shape(center, [face])


def circle_maker(center=Point()) -> Shape:
    vertices = []
    for i in range(0, 360, 1):
        vertices.append(Point(5 * math.cos(math.radians(i)), 5 * math.sin(math.radians(i))))
    face = Face(*vertices)
    return Shape(center, [face])


def input_point(prompt = ""):
    return Point(*[int(i) for i in input(prompt + "x, y: ").split(", ")])


renderer = Renderer(*screen_size)

# cube = circle_maker()
cube = box_maker()
cube.render(renderer)
# def e():
    # while True:
renderer.render()

# t = threading.Thread(target=e)
# t.start()

while True:
    print("1. moveBox\n2. scale\n3. extrude")
    option = input("Enter Option: ")
    if option == "1":
        cube.center += input_point()
    elif option == "2":
        inp = input("Amount: ")
        cube.scale(float(inp))
    elif option == "3":
        inp = input("1. face\n2. line\nchoice: ")
        face_id = int(input("\n".join([f"{i}: {face}" for i, face in enumerate(cube.faces)]) + "\nwhich face: "))
        if inp == '1':
            p = input_point("extruded_x, extruded_y: ")
            cube.extrude(face_id, p)
        else:
            line_id = int(input("\n".join([f"{i}: {face}" for i, face in enumerate(cube.faces[face_id].get_lines())]) + "\nwhich face: "))
            p = input_point("extruded_x, extruded_y: ")
            cube.extrude((face_id, line_id), p)
        # object_.faces[0] # [[l[i], l[(i+1)%4]] for i in range(4)] get lines
    renderer.reset_frame()
    cube.render(renderer)
    renderer.render()