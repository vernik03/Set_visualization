#include <SFML/Graphics.hpp>
#include <iostream>
#include <clocale>
#include <string>
#include <ctime>
#include <Windows.h>
#include <vector>
#include <cmath>   
#include <algorithm>
#include <fstream>

using namespace std;
using namespace sf;

vector<pair<int, int>> pairs_orig; //errors
vector<pair<int, int>> pairs2_orig;

vector<pair<int, int>> pairs; //errors
vector<pair<int, int>> pairs2;
vector<pair<int, int>> final;

vector<pair<pair<int, int>, pair<int, int>>> final_voronoy;

void drawWindow(RenderWindow* window, int key, RectangleShape* X, RectangleShape* Y);

int window_x = 1000;
int window_y = 1000;

int speed = 30;

struct Triangle
{
	pair<int, int> a;
	pair<int, int> b;
	pair<int, int> c;
	double radius;
	pair<double, double> center;
	vector<pair<pair<int, int>, pair<int, int>>> edges;
};

vector<Triangle> triangle_vector;

Triangle add_edges(Triangle temp) {
	temp.edges.push_back(make_pair(make_pair(temp.a.first, temp.a.second), make_pair(temp.b.first, temp.b.second)));
	temp.edges.push_back(make_pair(make_pair(temp.b.first, temp.b.second), make_pair(temp.c.first, temp.c.second)));
	temp.edges.push_back(make_pair(make_pair(temp.c.first, temp.c.second), make_pair(temp.a.first, temp.a.second)));
	return temp;
}


bool operator==(Triangle a, Triangle b) {
	if (a.a == b.a && a.b == b.b && a.c == b.c && a.center == b.center && a.radius == b.radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool operator==(pair<int, int> a, pair<int, int> b) {
	if (a.first == b.first && a.second == b.second)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool operator==(pair<pair<int, int>, pair<int, int>> a, pair<pair<int, int>, pair<int, int>> b) {
	if ((a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool compare(pair<int, int> p1, pair<int, int> p2) {
	return p1.second < p2.second;
}

double side(double ax, double ay, double bx, double by) {
	return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

double area_triangle(double ax, double ay, double bx, double by, double cx, double cy) {
	double ab = side(bx, by, ax, ay);
	double ac = side(cx, cy, ax, ay);
	double bc = side(bx, by, cx, cy);
	double p = (ab + ac + bc) / 2;
	double s = sqrt(p * (p - ab) * (p - ac) * (p - bc));
	double arccos = acos((bc * bc + ab * ab - ac * ac) / (2 * bc * ab));
	return arccos;

}
pair<double, double> find_center(Triangle temp) {
	double x;
	double y;
	double a1, a2, b1, b2, c1, c2;
	a1 = temp.a.first;
	a2 = temp.a.second;
	b1 = temp.b.first;
	b2 = temp.b.second;
	c1 = temp.c.first;
	c2 = temp.c.second;
	x = (((a1 * a1 + a2 * a2) * (b2 - c2) + (b1 * b1 + b2 * b2) * (c2 - a2) + (c1 * c1 + c2 * c2) * (a2 - b2)) / (2 * (a1 * (b2 - c2) + b1 * (c2 - a2) + c1 * (a2 - b2))));
	y = (((a1 * a1 + a2 * a2) * (c1 - b1) + (b1 * b1 + b2 * b2) * (a1 - c1) + (c1 * c1 + c2 * c2) * (b1 - a1)) / (2 * (a1 * (b2 - c2) + b1 * (c2 - a2) + c1 * (a2 - b2))));
	return make_pair(x, y);
}

double segment_length(pair<double, double> a, pair<double, double> b) {
	double a1, a2, b1, b2, result;
	a1 = a.first;
	a2 = a.second;
	b1 = b.first;
	b2 = b.second;
	result = sqrt((pow((a1 - b1), 2) + pow((a2 - b2), 2)));
	return result;
}

void to_final() {
	final.clear();
	for (int i = 0; i < triangle_vector.size(); i++)
	{
		final.push_back(triangle_vector[i].a);
		final.push_back(triangle_vector[i].b);
		final.push_back(triangle_vector[i].c);
		final.push_back(triangle_vector[i].a);
	}


}

void animate_delanay_step(Triangle super, vector<Triangle> badTriangles, vector<pair<pair<int, int>, pair<int, int>>> polygon, int i) {
	//std::cout << std::endl;
	badTriangles.clear();
	for (int t = 0; t < triangle_vector.size(); t++)
	{
		/*std::cout << pairs[i].first << " " << pairs[i].second << "    "
			<< triangle_vector[t].a.first << " " << triangle_vector[t].a.second << "    "
			<< triangle_vector[t].b.first << " " << triangle_vector[t].b.second << "    "
			<< triangle_vector[t].c.first << " " << triangle_vector[t].c.second << "    "
			<< triangle_vector[t].radius << " "
			<< triangle_vector[t].center.first << " " << triangle_vector[t].center.second << "\n";*/
		if (segment_length(pairs[i], triangle_vector[t].center) <= triangle_vector[t].radius)
		{
			badTriangles.push_back(triangle_vector[t]);
		}
	}
	polygon.clear();
	for (int t = 0; t < badTriangles.size(); t++)
	{
		//std::cout << "triangle " << t << "\n    ";
		for (int e = 0; e < 3; e++)
		{

			//std::cout << badTriangles[t].edges[e].first.first << " "<<badTriangles[t].edges[e].first.second << " " << badTriangles[t].edges[e].second.first << " " << badTriangles[t].edges[e].second.second << "\n";
			bool flag = 0;
			for (int j = 0; j < badTriangles.size(); j++)
			{
				if (t == j)
				{
					continue;
				}
				for (int r = 0; r < 3; r++)
				{

					if (badTriangles[t].edges[e] == badTriangles[j].edges[r])
					{
						//std::cout << "bad! collides with:   ";
						//std::cout << badTriangles[j].edges[r].first.first << " " << badTriangles[j].edges[r].first.second << " " << badTriangles[j].edges[r].second.first << " " << badTriangles[j].edges[r].second.second << "\n";
						flag = true;
					}
				}
			}
			if (!flag)
			{
				polygon.push_back(badTriangles[t].edges[e]);
			}
		}
	}
	//std::cout << "i=" << i << " active edges: \n";
	for (int i = 0; i < polygon.size(); i++) {
		//std::cout << polygon[i].first.first << " " << polygon[i].first.second << "  " << polygon[i].second.first << " " << polygon[i].second.second << "		  ";
	}
	for (int t = 0; t < badTriangles.size(); t++)
	{
		triangle_vector.erase(std::remove(triangle_vector.begin(), triangle_vector.end(), badTriangles[t]), triangle_vector.end());
	}
	for (int e = 0; e < polygon.size(); e++)
	{
		Triangle temp;
		temp.a = polygon[e].first;
		temp.b = polygon[e].second;
		temp.c = pairs[i];
		temp.center = find_center(temp);
		temp.radius = segment_length(temp.a, temp.center);
		temp = add_edges(temp);
		triangle_vector.push_back(temp);
	}
	/*std::cout << "\n\n\n";
	std::cout << "bad triangles\n";
	for (int j = 0; j < badTriangles.size(); j++) {
		std::cout << badTriangles[j].a.first << " " << badTriangles[j].a.second << " "
			<< badTriangles[j].b.first << " " << badTriangles[j].b.second << " "
			<< badTriangles[j].c.first << " " << badTriangles[j].c.second << "\n";
	}
	std::cout << "active triangles\n";
	for (int j = 0; j < triangle_vector.size(); j++) {
		std::cout << triangle_vector[j].a.first << " " << triangle_vector[j].a.second << " "
			<< triangle_vector[j].b.first << " " << triangle_vector[j].b.second << " "
			<< triangle_vector[j].c.first << " " << triangle_vector[j].c.second << "\n";
	}
	std::cout << "\n\n\n";*/
}

void calculate_delaunay_triangulation(RenderWindow* window, RectangleShape* X, RectangleShape* Y, bool voronoy = 0) {
	triangle_vector.clear();
	Triangle super;
	super.a.first = 0;
	super.a.second = (sqrt(3 / 2) * window_x + window_x / 2) * 25;
	super.b.first = (window_x / sqrt(3 / 2) + window_y / 2) * 25;
	super.b.second = (-window_y / 2) * 25;
	super.c.first = -(window_x / sqrt(3 / 2) + window_y / 2) * 25;
	super.c.second = (-window_y / 2) * 25;
	super.center = find_center(super);
	super.radius = segment_length(super.center, super.a);
	super = add_edges(super);
	triangle_vector.push_back(super);
	vector<Triangle> badTriangles;
	vector<pair<pair<int, int>, pair<int, int>>> polygon;
	for (int i = 0; i < pairs.size(); i++)
	{
		animate_delanay_step(super, badTriangles, polygon, i);
		Sleep(speed*2);
		drawWindow(window, 2, X, Y);
	}
	vector<Triangle> to_del;

	for (int i = 0; i < triangle_vector.size(); i++)
	{
		/*if ((segment_length(triangle_vector[i].a, super.center) > super.radius) ||
			(segment_length(triangle_vector[i].b, super.center) > super.radius) ||
			(segment_length(triangle_vector[i].c, super.center) > super.radius))*/
		bool flag = false;
		for (int a = 0; a < 3; a++) {
			for (int b = 0; b < 3; b++) {
				if (triangle_vector[i].edges[a] == super.edges[b]) {
					flag = true;
				}
			}
		}

		if (flag)
		{
			to_del.push_back(triangle_vector[i]);
		}

	}
	for (int i = 0; i < triangle_vector.size(); i++)
	{
		if ((triangle_vector[i].a.first > 500 || triangle_vector[i].a.first < -500) ||
			(triangle_vector[i].b.first > 500 || triangle_vector[i].b.first < -500) ||
			(triangle_vector[i].c.first > 500 || triangle_vector[i].c.first < -500) ||
			(triangle_vector[i].a.second > 500 || triangle_vector[i].a.second < -500) ||
			(triangle_vector[i].b.second > 500 || triangle_vector[i].b.second < -500) ||
			(triangle_vector[i].c.second > 500 || triangle_vector[i].c.second < -500))
		{
			to_del.push_back(triangle_vector[i]);
		}
	}
	if (!voronoy) {
		for (int i = 0; i < to_del.size(); i++)
		{
			triangle_vector.erase(std::remove(triangle_vector.begin(), triangle_vector.end(), to_del[i]), triangle_vector.end());
			Sleep(speed/2);
			drawWindow(window, 2, X, Y);

		}
	}
	//Sleep(1000);
	//std::cout << "final triangles\n";
	/*for (int j = 0; j < triangle_vector.size(); j++) {
		std::cout << triangle_vector[j].a.first << " " << triangle_vector[j].a.second << " "
			<< triangle_vector[j].b.first << " " << triangle_vector[j].b.second << " "
			<< triangle_vector[j].c.first << " " << triangle_vector[j].c.second << "\n";
	}*/
	//std::cout << "\n\n\n";
	drawWindow(window, 2, X, Y);
	to_final();
}

void clean() {
	vector<Triangle> to_del;
	for (int i = 0; i < triangle_vector.size(); i++)
	{
		if ((triangle_vector[i].a.first > 500 || triangle_vector[i].a.first < -500) ||
			(triangle_vector[i].b.first > 500 || triangle_vector[i].b.first < -500) ||
			(triangle_vector[i].c.first > 500 || triangle_vector[i].c.first < -500) ||
			(triangle_vector[i].a.second > 500 || triangle_vector[i].a.second < -500) ||
			(triangle_vector[i].b.second > 500 || triangle_vector[i].b.second < -500) ||
			(triangle_vector[i].c.second > 500 || triangle_vector[i].c.second < -500))
		{
			to_del.push_back(triangle_vector[i]);
		}
	}
	for (int i = 0; i < to_del.size(); i++)
	{
		triangle_vector.erase(std::remove(triangle_vector.begin(), triangle_vector.end(), to_del[i]), triangle_vector.end());

	}
}

void calculate_convex_set(RenderWindow* window, RectangleShape* X, RectangleShape* Y) {
	//ifstream in;
	//in.open("Text.txt");
	int x, y, t, n;
	int j = 0;



	vector<int> yvV;
	vector<int> xv;
	vector<int> yv;
	char ch;

	srand(time(NULL));
	/*	for (int i = 0; i < 350; i++)
		{
			x = rand() % 500-250;
			y = rand() % 500-250;
			pairs.push_back(make_pair(x, y));
			pairs2.push_back(make_pair(x, y));
		}*/

		/*for (int i = 0; i < pairs.size(); i++)
		{

			cout << pairs.at(i).first << " / " << pairs.at(i).second << endl;

		}*/

		//cout << "\n";


	int xend, yend;
	const auto p = min_element(pairs.begin(), pairs.end(), compare);
	auto x1 = p->first;
	auto y1 = p->second;
	//cout << x1 << " / " << y1 << endl;
	//cout << "\n";

	pairs.erase(remove(pairs.begin(), pairs.end(), make_pair(x1, y1)), pairs.end());
	pairs.push_back(make_pair(x1, y1));
	xend = x1;
	yend = y1;
	/*for (int i = 0; i < pairs.size(); i++)
	{

		cout << pairs.at(i).first << " / " << pairs.at(i).second << endl;

	}*/
	//cout << "\n";
	for (int i = 0; i < pairs.size(); i++)
	{
		const auto p = min_element(pairs.begin(), pairs.end(), compare);
		auto x2 = p->first;
		auto y2 = p->second;


		if (y2 == y1)
		{
			if (x2 < x1)
			{
				pairs.erase(remove(pairs.begin(), pairs.end(), make_pair(x2, y2)), pairs.end());
				pairs.push_back(make_pair(x2, y2));
				//cout << x2 << " / " << y2 << endl;
				//cout << "\n";
				xend = x2;
				yend = y2;
			}
		}

	}
	//	cout << "\n";
	/*	for (int i = 0; i < pairs.size(); i++)
		{

			cout << pairs.at(i).first << " / " << pairs.at(i).second << endl;

		}*/
		//	cout << "\n";
			//cout << xend << " / " << yend << endl;
	pairs.pop_back();


	int xx1, yy1;



	double sign;
	vector<float> cosinus1;
	for (int i = 0; i < pairs.size(); i++)
		if (pairs.at(i).first != xend || pairs.at(i).second != yend)
		{
			sign = area_triangle(xend - 1, yend, xend, yend, pairs.at(i).first, pairs.at(i).second);

			if (pairs.at(i).first != xend && pairs.at(i).second != yend && sign > 0) {
				cosinus1.push_back(sign);
			}


		}

	//	cout << "\n";

	const auto css = max_element(cosinus1.begin(), cosinus1.end());

	int num1 = distance(cosinus1.begin(), css);
	xx1 = pairs.at(num1).first;
	yy1 = pairs.at(num1).second;

	//	cout << "\n";
	//	cout << "\n";
	//	cout << "\n";

	pairs.erase(remove(pairs.begin(), pairs.end(), make_pair(xx1, yy1)), pairs.end());

	//	cout << xx1 << " / " << yy1 << endl;

	final.push_back(make_pair(xend, yend));
	final.push_back(make_pair(xx1, yy1));
	pairs.push_back(make_pair(xend, yend));

	double ax = xend;
	double ay = yend;
	double bx = xx1;
	double by = yy1;

	while (true)
	{
		Sleep(speed);
		drawWindow(window, 1, X, Y);
		vector<float> cosinus;
		for (int i = 0; i < pairs.size(); i++)
		{
			double  cx = pairs.at(i).first;
			double  cy = pairs.at(i).second;
			double cosss = area_triangle(ax, ay, bx, by, cx, cy);
			cosinus.push_back(cosss);

		}

		//cout << "\n";

		const auto cs = max_element(cosinus.begin(), cosinus.end());

		int num = distance(cosinus.begin(), cs);

		ax = bx;
		ay = by;
		bx = pairs.at(num).first;
		by = pairs.at(num).second;
		//cout << pairs.at(num).first << " / " << pairs.at(num).second << endl;
		//cout << "\n";
		if (pairs.at(num).first == xend && pairs.at(num).second == yend)
		{
			break;
			//goto end;
		}

		final.push_back(make_pair(pairs.at(num).first, pairs.at(num).second));
		pairs.erase(pairs.begin() + num);

		cosinus.clear();

	}

	//end:;

	final.push_back(make_pair(xend, yend));
	//cout << "\n";
	/*for (int i = 0; i < pairs.size(); i++)
	{

		cout << pairs.at(i).first << " / " << pairs.at(i).second << endl;

	}
	cout << "\n";

	for (int i = 0; i < final.size(); i++)
	{

		cout << final.at(i).first << " / " << final.at(i).second << endl;

	}*/

}

void calculate_voronoi_diagram(RenderWindow* window, RectangleShape* X, RectangleShape* Y) {
	final.clear();
	pairs.clear();
	pairs2.clear();
	pairs = pairs_orig;
	pairs2 = pairs_orig;
	calculate_delaunay_triangulation(window, X, Y, 1);//triangle_vector
	//final_voronoy

	final_voronoy.clear();
	//for (int j = 0; j < triangle_vector.size(); j++) {
		//std::cout << "triangle " << j << "\n";
		//std::cout << triangle_vector[j].a.first << " " << triangle_vector[j].a.second << " "
			//<< triangle_vector[j].b.first << " " << triangle_vector[j].b.second << " "
			//<< triangle_vector[j].c.first << " " << triangle_vector[j].c.second << "\n";
		//for (int e = 0; e < 3; e++)
		//{

		//	//std::cout << triangle_vector[j].edges[e].first.first << " " << triangle_vector[j].edges[e].first.second << " " << triangle_vector[j].edges[e].second.first << " " << triangle_vector[j].edges[e].second.second << "\n";
		//}
		//std::cout << "center " << triangle_vector[j].center.first << " " << triangle_vector[j].center.second << "\n";
	//}
	for (int i = 0; i < triangle_vector.size(); i++)
	{
		for (int j = i + 1; j < triangle_vector.size(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (triangle_vector[i].edges[k] == triangle_vector[j].edges[0] || triangle_vector[i].edges[k] == triangle_vector[j].edges[1] || triangle_vector[i].edges[k] == triangle_vector[j].edges[2])
				{
					final_voronoy.push_back(make_pair(triangle_vector[i].center, triangle_vector[j].center));
					Sleep(speed);
					drawWindow(window, 3, X, Y);
				}
			}
		}
	}
	//for (auto elem : final_voronoy) {
		//std::cout << elem.first.first << " " << elem.first.second << " " << elem.second.first << " " << elem.second.second << "\n";
//	}
	//std::cout << "\n";
	drawWindow(window, 3, X, Y);
}

void drawWindow(RenderWindow* window, int key, RectangleShape* X, RectangleShape* Y) {
	window->clear(Color(10, 25, 40, 0));
	Vertex background[] =
	{
			Vertex(Vector2f(0, 0), Color(159, 226, 255)),
			Vertex(Vector2f(0, 1000),  Color(255, 185, 185)),
			Vertex(Vector2f(1000, 1000), Color(255, 226, 255)),
			Vertex(Vector2f(1000, 0), Color(90, 255, 255))
	};
	window->draw(background, 4, sf::Quads);
	VertexArray lines(sf::LinesStrip, final.size());
	VertexArray lines_for_triangle(sf::LinesStrip, 4);
	if (key == 2)
	{
		for (int i = 0; i < triangle_vector.size(); i++)
		{
			lines_for_triangle[0].position = Vector2f(500 + triangle_vector.at(i).a.first, 500 - triangle_vector.at(i).a.second);
			lines_for_triangle[1].position = Vector2f(500 + triangle_vector.at(i).b.first, 500 - triangle_vector.at(i).b.second);
			lines_for_triangle[2].position = Vector2f(500 + triangle_vector.at(i).c.first, 500 - triangle_vector.at(i).c.second);
			lines_for_triangle[3].position = Vector2f(500 + triangle_vector.at(i).a.first, 500 - triangle_vector.at(i).a.second);

			lines_for_triangle[0].color = Color(10, 25, 40);
			lines_for_triangle[1].color = Color(10, 25, 40);
			lines_for_triangle[2].color = Color(10, 25, 40);
			lines_for_triangle[3].color = Color(10, 25, 40);

			window->draw(lines_for_triangle);

		}
	}
	VertexArray lines_for_voronoy(sf::LinesStrip, 2);

	if (key == 3)
	{
		for (int i = 0; i < final_voronoy.size(); i++)
		{
			lines_for_voronoy[0].position = Vector2f(500 + final_voronoy.at(i).first.first, 500 - final_voronoy.at(i).first.second);
			lines_for_voronoy[1].position = Vector2f(500 + final_voronoy.at(i).second.first, 500 - final_voronoy.at(i).second.second);

			lines_for_voronoy[0].color = Color(255, 0, 0);
			lines_for_voronoy[1].color = Color(255, 0, 0);

			window->draw(lines_for_voronoy);

		}
	}

	sf::ConvexShape convex;
	convex.setPointCount(final.size());
	for (int i = 0; i < pairs2.size(); i++)
	{

		CircleShape circle3(5.f);
		circle3.setOrigin(5, 5);
		circle3.setFillColor(Color(44, 110, 176));
		circle3.move(500 + pairs2.at(i).first, 500 - pairs2.at(i).second);
		window->draw(circle3);
	}
	VertexArray lines_for_1(sf::LinesStrip, 2);
	if (key == 1)
	{
		if (final.size() > 2)
		{
			for (int i = 0; i < final.size() - 1; i++)
			{
				/*float xxx = 500 + final.at(i).first;
				float yyy = 500 - final.at(i).second;
				convex.setPoint(i, sf::Vector2f(500 + final.at(i).first, 500 - final.at(i).second));
				convex.setFillColor(sf::Color(255, 255, 255, 0));
				convex.setOutlineThickness(3);
				convex.setOutlineColor(sf::Color(10, 25, 40));
				lines[i].position = Vector2f(500 + final.at(i).first, 500 - final.at(i).second);

				lines[i].color = Color(10, 25, 40);
				CircleShape circle2(7.f);
				circle2.setOrigin(7, 7);
				circle2.setFillColor(Color(10, 25, 40));
				circle2.move(500 + final.at(i).first, 500 - final.at(i).second);
				window.draw(circle2);*/

				lines_for_1[0].position = Vector2f(500 + final.at(i).first, 500 - final.at(i).second);
				lines_for_1[1].position = Vector2f(500 + final.at(i + 1).first, 500 - final.at(i + 1).second);

				lines_for_1[0].color = Color(10, 25, 40);
				lines_for_1[1].color = Color(10, 25, 40);

				window->draw(lines_for_1);
			}
			lines_for_1[0].position = Vector2f(500 + final.at(final.size() - 1).first, 500 - final.at(final.size() - 1).second);
			lines_for_1[1].position = Vector2f(500 + final.at(0).first, 500 - final.at(0).second);

			lines_for_1[0].color = Color(10, 25, 40);
			lines_for_1[1].color = Color(10, 25, 40);

			window->draw(lines_for_1);
		}
	}
	/*
	for (int i = 0; i < triangle_vector.size(); i++)
	{
					ConvexShape convex1;
		convex1.setPointCount(3);
		convex1.setPoint(0, Vector2f(500 + triangle_vector.at(i).a.first, 500 - triangle_vector.at(i).a.second));
		convex1.setPoint(1, Vector2f(500 + triangle_vector.at(i).b.first, 500 - triangle_vector.at(i).b.second));
		convex1.setPoint(2, Vector2f(500 + triangle_vector.at(i).c.first, 500 - triangle_vector.at(i).c.second));
		convex1.setOutlineThickness(3.f);
		convex1.setFillColor(Color(0,0,0,0));
		convex1.setOutlineColor(Color(44, 110, 176));
		window.draw(convex1);
	}*/



	//window.draw(lines);
	window->draw(convex);
	window->draw(*Y);
	window->draw(*X);

	window->display();
}


void random() {
	pairs_orig.clear();
	for (int i = 0; i < 100; i++)
	{
		int x = rand() % 750 - 375;
		int y = rand() % 750 - 375;
		pairs_orig.push_back(make_pair(x, y));
	}
}

int main()
{

	srand(time(NULL));
	cout << "Click to add a point." << endl
		<< "Press '1' to show convex hull." << endl
		<< "Press '2' to show Delaunay triangulation." << endl
		<< "Press '3' to show Voronoi diagram." << endl
		<< "Press 'R' to randomize numbers." << endl
		<< "Press 'F' to read numbers from file." << endl
		<< "Press '+' to increase speed." << endl
		<< "Press '-' to reduce speed." << endl
		<< "Press '0' to make speed approximately equal to 0." << endl;


	RenderWindow window(VideoMode(window_x, window_y), "SFML Works!");

	CircleShape circle(3.f);
	circle.setFillColor(Color(10, 25, 40));
	circle.move(15, 15);

	RectangleShape Y(Vector2f(1000.f, 2.f));
	Y.setFillColor(Color(10, 25, 40));
	Y.rotate(90.f);
	Y.move(500, 0);

	RectangleShape X(Vector2f(1000.f, 2.f));
	X.setFillColor(Color(10, 25, 40));
	X.move(0, 500);

	int key = 1;

	/*pairs_orig.push_back(std::pair<int, int>(-300, -300));
	pairs_orig.push_back(std::pair<int, int>(-300, 200));
	pairs_orig.push_back(std::pair<int, int>(200, -300));
	pairs_orig.push_back(std::pair<int, int>(200, 200));
	pairs_orig.push_back(std::pair<int, int>(50, 180));*/

	while (window.isOpen())
	{

		Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					key = 0;
					return 0;
				}
				if (event.key.code == sf::Keyboard::Num1)
				{
					key = 1;
					if (pairs_orig.size() > 3)
					{
						final.clear();
						pairs.clear();
						pairs2.clear();
						pairs = pairs_orig;
						pairs2 = pairs_orig;
						calculate_convex_set(&window, &X, &Y);
					}

				}
				if (event.key.code == sf::Keyboard::Num2)
				{
					key = 2;
					if (pairs_orig.size() > 3)
					{
						final.clear();
						pairs.clear();
						pairs2.clear();
						pairs = pairs_orig;
						pairs2 = pairs_orig;
						calculate_delaunay_triangulation(&window, &X, &Y);
						clean();
					}
				}
				if (event.key.code == sf::Keyboard::Num3)
				{
					key = 3;
					if (pairs_orig.size() > 3)
					{
						final.clear();
						pairs.clear();
						pairs2.clear();
						pairs = pairs_orig;
						pairs2 = pairs_orig;
						calculate_voronoi_diagram(&window, &X, &Y);
					}

				}
				if (event.key.code == sf::Keyboard::R)
				{

					random();
					final.clear();
					pairs.clear();
					pairs2.clear();
					pairs = pairs_orig;
					pairs2 = pairs_orig;
					if (key == 1)
					{
						calculate_convex_set(&window, &X, &Y);
					}
					if (key == 2)
					{
						calculate_delaunay_triangulation(&window, &X, &Y);
						clean();
					}
					if (key == 3)
					{
						calculate_voronoi_diagram(&window, &X, &Y);
					}

				}
				if (event.key.code == sf::Keyboard::F)
				{
					pairs_orig.clear();
					ifstream in;
					in.open("List.txt");
					int x, y;
					while (!in.eof()) {
						in >> x;
						in >> y;
						pairs_orig.push_back(make_pair(x, y));
					}
					in.close();
					final.clear();
					pairs.clear();
					pairs2.clear();
					pairs = pairs_orig;
					pairs2 = pairs_orig;
					if (key == 1)
					{
						calculate_convex_set(&window, &X, &Y);
					}
					if (key == 2)
					{
						calculate_delaunay_triangulation(&window, &X, &Y);
						clean();
					}
					if (key == 3)
					{
						calculate_voronoi_diagram(&window, &X, &Y);
					}

				}
				if (event.key.code == sf::Keyboard::Equal)
				{
					speed += 30;
				}
				if (event.key.code == sf::Keyboard::Num0)
				{
					speed = 0;
				}
				if (event.key.code == sf::Keyboard::Dash)
				{
					if (speed>35)
					{
						speed -= 30;
					}
					else
					{
						speed = 0;

					}
				
				}

			}

			if (event.type == event.MouseButtonReleased && event.mouseButton.button == Mouse::Left) {

				pairs_orig.push_back(make_pair(Mouse::getPosition(window).x - 500, -(Mouse::getPosition(window).y - 500)));

				//cout << pairs_orig[pairs_orig.size() - 1].first << "/" << pairs_orig[pairs_orig.size() - 1].second << endl;

				if (pairs_orig.size() > 3)
				{
					final.clear();
					pairs.clear();
					pairs2.clear();
					pairs = pairs_orig;
					pairs2 = pairs_orig;
					if (key == 1)
					{
						calculate_convex_set(&window, &X, &Y);
					}
					if (key == 2)
					{
						calculate_delaunay_triangulation(&window, &X, &Y);
						clean();
					}
					if (key == 3)
					{
						calculate_voronoi_diagram(&window, &X, &Y);
					}
					if (key == 0)
					{
						return 0;
					}

				}
				else
				{

					pairs = pairs_orig;
					pairs2 = pairs_orig;
					final = pairs_orig;

				}
			}

			if (event.type == Event::Closed)

				window.close();
		}
		drawWindow(&window, key, &X, &Y);
	}

	return 0;
}