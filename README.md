# OutLine
# Description
This program is designed to find a subset of points in a two-dimensional space that surrounds all other points while forming a polygon with the maximum area. The program is based on the Chan's algorithm, which is currently the most optimized algorithm for this task. This algorithm combines two other algorithms: Graham scan and Jarvis march. This combination results in a time complexity of O(nlog(h)), where n is the number of points and h is the number of vertices in the convex hull.

# Usage
Input:

The program takes a set of points in a two-dimensional space as input. (in form of .txt file like in file points20.txt)

Output:

It returns a subset of points forming the convex hull with the maximum area.

# Visualization
In visualization directory there is a python script wich makes visualizations from files in.txt and out.txt.
![image](https://github.com/AitenAndGo/OutLine/assets/87769038/8663546a-8938-4893-89be-e7a7e2cf615a)
