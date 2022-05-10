#include <iostream>
#include <string>
using namespace std;

class Circle;
class Square;

class ShapeVisitor
{
public:
	virtual ~ShapeVisitor() = default;
	virtual void Visit(Circle*) = 0;
	virtual void Visit(Square*) = 0;
};

class Shape
{
public:
	virtual ~Shape() = default;
	virtual float Size() const = 0;
	virtual float Area() const = 0;
	virtual void Accept(ShapeVisitor* v) = 0;
};

class Circle : public Shape
{
public:
	explicit Circle(float r) : radius(r) {}
	virtual float Size() const override { return radius; }
	virtual float Area() const override { return 3.14159f * radius * radius; }
	virtual void Accept(ShapeVisitor* v) override { v->Visit(this); }
private:
	float radius;
};

class Square : public Shape
{
public:
	explicit Square(float s) : side(s) {}
	virtual float Size() const override { return side; }
	virtual float Area() const override { return side * side; }
	virtual void Accept(ShapeVisitor* v) override { v->Visit(this); }
private:
	float side;
};



class SerializeShapeVisitor : public ShapeVisitor
{
public:
	virtual void Visit(Circle* c) override { cout << "Serializing Circle, Radius = " << c->Size() << ", Area = " << c->Area() << endl; }
	virtual void Visit(Square* s) override { cout << "Serializing Suqare, Size = " << s->Size() << ", Area = " << s->Area() << endl; }
};

class RenderShapeVisitor : public ShapeVisitor
{
public:
	virtual void Visit(Circle* c) override { cout << "Rendering Circle, Radius = " << c->Size() << ", Area = " << c->Area() << endl; }
	virtual void Visit(Square* s) override { cout << "Rendering Suqare, Size = " << s->Size() << ", Area = " << s->Area() << endl; }
};



int main()
{
	Shape* c = new Circle(1.5f);
	Shape* s = new Square(2.2f);

	ShapeVisitor* sv = new SerializeShapeVisitor;
	c->Accept(sv);
	s->Accept(sv);

	ShapeVisitor* rv = new RenderShapeVisitor;
	c->Accept(rv);
	s->Accept(rv);

	// sv->Visit(c);
	// rv->Visit(s);
}
