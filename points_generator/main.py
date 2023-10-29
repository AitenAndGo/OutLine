import random

def generate_points_gaussian(num_points, mean, std_dev):
    points = [(random.gauss(mean, std_dev), random.gauss(mean, std_dev)) for _ in range(num_points)]
    return points

def save_points_to_file(points, filename):
    with open(filename, 'w') as file:
        for point in points:
            file.write(f"{point[0]} {point[1]}\n")

num_points = 80000
mean = 0  # Średnia rozkładu normalnego
std_dev = 350  # Odchylenie standardowe rozkładu normalnego
points = generate_points_gaussian(num_points, mean, std_dev)
save_points_to_file(points, 'in.txt')
