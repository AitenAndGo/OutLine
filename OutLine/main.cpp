#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <algorithm>
#include <ctime>

struct Point
{
	float x;
	float y;
	float polar_angle;

	Point() {};
	Point(float _x, float _y) : x(_x), y(_y) {}
	bool operator==(const Point& drugi) {
		if (drugi.x == x && drugi.y == y) {
			return true;
		}
		return false;
	}
	Point& operator=(const Point& drugi) {
		this->x = drugi.x;
		this->y = drugi.y;
		this->polar_angle = drugi.polar_angle;
		return *this;
	}
};

bool readFile(const std::string fileName, std::vector<Point>& points);
bool calculateConvexHull(std::vector<Point>& points, int m, int n, std::vector<Point>& outLinePoints);
void GrahamScan(std::vector<Point>&points);
Point jarvisMarch(std::vector<Point>& points, Point p0, Point p);
float calculateAngle(Point a, Point b);
float calculateAngle(Point a, Point b, Point c);
bool compare_angle(Point a, Point b);
float distance(Point a, Point b);
float angle_dir(Point a, Point b, Point c);

int main(void) {


	int m = 2;
	int t = 0;
	int numberOfPoints;

	std::vector <Point> points;
	std::vector <Point> outLinePoints;

	std::string fileName;
	std::cout << "Enter file name: " << std::endl;
	std::cin >> fileName;

	if (!readFile(fileName, points)) {
		std::cout << "\nsomething wrong with the file!!! (try \"NameOfFile.txt\")\n";
		return 1;
	}
	
	numberOfPoints = points.size();

	while (!calculateConvexHull(points, m, numberOfPoints, outLinePoints)) {
		if (m >= numberOfPoints) {
			std::cout << "program has failed" << std::endl;
			return 2;
		}
		t++;
		m = std::min(numberOfPoints, (int)pow(2, (pow(2, t))));
	}

	std::cout << "ConvexHull:" << std::endl;
	for (int i = 0; i < outLinePoints.size(); i++){
		std::cout << outLinePoints[i].x << " " << outLinePoints[i].y << std::endl;
	}

	return 0;
}

bool readFile(const std::string fileName, std::vector<Point>& points) {
	
	std::ifstream textFile(fileName);
	if (textFile.is_open()) {
		//read points from file
		int numOfElements;
		textFile >> numOfElements;
		for (int i = 0; i < numOfElements; i++){
			float x, y;
			textFile >> x;
			textFile.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
			textFile >> y;

			Point point;
			point.x = x;
			point.y = y;
			points.push_back(point);
		}
	} else{
		return false;
	}
	return true;
}

bool calculateConvexHull(std::vector<Point>& points, int m, int n, std::vector<Point>& outLinePoints)
{
	std::vector<Point> boundryPoints;

	//divide the points into k sets of m points each
	int k = ceil((float)n/(float)m);
	int a = 0;
	std::vector<std::vector<Point>> sets(k);

	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < m; j++) {
			if (j + i * m >= points.size())
				break;
			Point p = points[j + (i * m)];
			//keep the point with the the lowest y value at the top
			if (!sets[i].empty()) {
				if (p.y < sets[i].back().y) {
					sets[i].push_back(p);
				}else if (p.y == sets[i].back().y) {
					if (p.x <= sets[i][0].x){
						sets[i].push_back(p);
					}
				}else{
					Point temp = sets[i].back();
					sets[i].pop_back();
					sets[i].push_back(p);
					sets[i].push_back(temp);
				}
			}
			else
			{
				sets[i].push_back(p);
			}
		}
		// calculate the convex hull of the set 'i' using Graham's scan algorithm
		GrahamScan(sets[i]);
	}

	//calculate the convex hull from each small polygon using Jarvis march algorithm (binary)


	//point with the smallest y value
	
	Point p1 = points[0];
	for (int i = 0; i < k; i++)
	{
		if (sets[i][0].y < p1.y) {
			p1 = sets[i][0];
		}
		else if (sets[i][0].y == p1.y)
		{
			if (sets[i][0].x < p1.x) {
				p1 = sets[i][0];
			}
		}
	}
	Point p0(-INFINITY, p1.y);
	boundryPoints.push_back(p0);
	boundryPoints.push_back(p1);

	//for all boundry points
	for (int i = 1; i < m; i++)
	{
		//for all sets
		Point nextPoint;
		float angle = 0;
		for (int j = 0; j < k; j++)
		{
			//calculate the point that forms the largest interior angle with the current edge 
			Point q = jarvisMarch(sets[j], boundryPoints[i - 1], boundryPoints[i]);

			//keep Point that forms the largest angle
			float tempAngle = calculateAngle(boundryPoints[i - 1], boundryPoints[i], q);
			if (j == 0) {
				angle = tempAngle;
				nextPoint = q;
			}
			else if (tempAngle < angle) {
				nextPoint = q;
				angle = tempAngle;
			}
		}
		if (nextPoint == boundryPoints[1]) {
			outLinePoints = boundryPoints;
			outLinePoints.erase(outLinePoints.begin());
			return true;
		}
		boundryPoints.push_back(nextPoint);
	}

	return false;
}

void GrahamScan(std::vector<Point>& points) {
	if (points.size() < 3)
		return;
	//sort points
	std::vector<Point> stack;
	Point p = points.back();
	for (int i = 0; i < points.size(); i++){
		points[i].polar_angle = calculateAngle(p, points[i]);
	}
	std::sort(points.begin(), points.end(), compare_angle);

	//find outline 
	stack.push_back(points[0]);
	stack.push_back(points[1]);
	for (int i = 2; i < points.size(); i++)
	{
		//if makes a counterclockwise turn add to stack
		while (true)
		{
			float dir = angle_dir(stack[stack.size() - 2], stack[stack.size() - 1], points[i]);
			if (dir > 0) {
				stack.push_back(points[i]);
				break;
			} 
			//3 points are at the same line
			else if(dir == 0){
				//delete middle point
				if (distance(stack[stack.size() - 2], stack[stack.size() - 1]) < distance(stack[stack.size() - 1], points[i])) {
					stack.pop_back();
					stack.push_back(points[i]);
					break;
				}
				break;
			}
			stack.pop_back();
		}
	}
	points = stack;
}

float angle_dir(Point a, Point b, Point c) {
	Point vector_a(b.x - a.x, b.y - a.y);
	Point vector_b(c.x - b.x, c.y - b.y);
	return (vector_a.x * vector_b.y) - (vector_b.x * vector_a.y);
}

float distance(Point a, Point b) {
	return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

bool compare_angle(Point a, Point b) {
	return a.polar_angle < b.polar_angle;
}

float calculateAngle(Point a, Point b) {
	float angle = atan2(b.y - a.y, b.x - a.x);
	if (angle < 0) {
		angle += 2 * 3.1415;
	}
	return angle;
}

float calculateAngle(Point a, Point b, Point c) {
	if (a == c || b == c || a == b) {
		return INFINITY;
	}
	float angle_AB = atan2(b.y - a.y, b.x - a.x);
	float angle_BC = atan2(c.y - b.y, c.x - b.x);

	float angle = angle_BC - angle_AB;

	if (angle < 0) {
		angle += 2 * 3.1415;
	}

	return angle * (180.0 / 3.1415);
}

Point jarvisMarch(std::vector<Point>& points, Point p0, Point p) {

	//standart search:
	float angle = calculateAngle(p0, p, points[0]);
	int index = 0;
	for (int i = 1; i < points.size(); i++) {
		float tempAngle = calculateAngle(p0, p, points[i]);
		if (tempAngle < angle) {
			angle = tempAngle;
			index = i;
		}
	}
	return points[index];


	//binary search: (to do)
	//int i = 0;
	//int j = points.size() / 2;
	//int k;
	//if (calculateAngle(p0, p, points.back()) > calculateAngle(p0, p, points[i]) && calculateAngle(p0, p, points[i]) < calculateAngle(p0, p, points[i + 1])) {
	//	return points[i];
	//} else if (calculateAngle(p0, p, points[j - 1]) > calculateAngle(p0, p, points[j]) && calculateAngle(p0, p, points[j]) < calculateAngle(p0, p, points[j + 1])){
	//	return points[j];
	//} else{
	//	while (true)
	//	{
	//		Point in;
	//		Point jn;
	//		Point ip;
	//		Point jp;
	//		//find half with smallest angle
	//		i - 1 < 0 ? in = points.back() : points[i - 1];
	//		j - 1 < 0 ? jn = points.back() : points[j - 1];
	//		i + 1 >= points.size() ? ip = points[0] : points[i + 1];
	//		j + 1 >= points.size() ? jp = points[0] : points[j + 1];

	//		if (calculateAngle(p0, p, in) < calculateAngle(p0, p, points[i]) && calculateAngle(p0, p, points[i]) < calculateAngle(p0, p, ip)) {
	//			if (calculateAngle(p0, p, jn) > calculateAngle(p0, p, points[j]) && calculateAngle(p0, p, points[j]) > calculateAngle(p0, p, jp)) {
	//				k = (j / 2) - 1;
	//			}
	//		}
	//		else if (calculateAngle(p0, p, in) > calculateAngle(p0, p, points[i]) && calculateAngle(p0, p, points[i]) > calculateAngle(p0, p, ip)) {
	//			if (calculateAngle(p0, p, jn) < calculateAngle(p0, p, points[j]) && calculateAngle(p0, p, points[j]) < calculateAngle(p0, p, jp)) {
	//				k = (j + (j / 2)) - 1;
	//			}
	//		}
	//		else if (calculateAngle(p0, p, in) > calculateAngle(p0, p, points[i]) && calculateAngle(p0, p, points[i]) > calculateAngle(p0, p, ip)) {
	//			if (calculateAngle(p0, p, jn) < calculateAngle(p0, p, points[j]) && calculateAngle(p0, p, points[j]) < calculateAngle(p0, p, jp)) {
	//				k = (j + (j / 2)) - 1;
	//			}
	//		}
	//	}
	//}
}
