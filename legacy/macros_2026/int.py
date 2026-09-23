import numpy as np
import matplotlib.pyplot as plt

# 定义参数
phi = 8  # 束斑直径 (mm)
R = phi / 2  # 束斑的半径 (mm)
sigma = phi / (2 * np.sqrt(2 * np.log(2)))  # 高斯分布标准差
# sigma = phi / 6  # 高斯分布标准差，取2.35sigma(FWHM)为束斑范围
w = 70  # 探测窗口宽度 (mm)
h = 55  # 探测窗口高度 (mm)
rd = 60.15  # 探测器与靶的距离 (mm)

# rd = 160.15  # 探测器与靶的距离 (mm)

w = 50
h = 50
rd = 93

theta_c = 0 / 180 * np.pi  # 探测窗口中心角度 (rad)
# theta_c_range = np.linspace(-np.pi/8, np.pi/8, 100)  # 中心角度的范围
n_particles = 1000000  # 模拟出射粒子数目


# 以二维高斯分布在束斑范围内抽样一点 (x, y)
def sample_point_from_gaussian(sigma):
    x = np.random.normal(0, sigma)
    y = np.random.normal(0, sigma)
    return x, y


# 抽取一个随机方向单位向量
def sample_direction():
    theta = np.random.uniform(0, 2 * np.pi)
    cospsi = np.random.uniform(-1, 1)
    psi = np.arccos(cospsi)
    return np.array(
        [np.sin(psi) * np.cos(theta), np.sin(psi) * np.sin(theta), np.cos(psi)]
    )


# 各项同性发射，判断是否通过探测窗口，通过则记录角度
def is_in_window(sigma, w, h, rd, theta_c):
    # 在束斑上采样一个点 (x, y)
    x, y = sample_point_from_gaussian(sigma)
    # 抽样一个随机方向单位向量
    direction = sample_direction()
    # 将束斑采样点视为原点坐标偏移，计算探测器窗口的角度范围，向上为y，向我为x，束流方向为z
    ydthetamin = np.pi / 2 - np.arctan((h / 2 - y) / rd)
    ydthetamax = np.pi / 2 + np.arctan((h / 2 + y) / rd)
    zdthetamin = theta_c - np.arctan(
        (w / 2 + x * np.cos(theta_c)) / (rd - x * np.sin(theta_c))
    )
    zdthetamax = theta_c + np.arctan(
        (w / 2 - x * np.cos(theta_c)) / (rd - x * np.sin(theta_c))
    )
    # 考虑束斑采样，判断方向是否在探测器窗口内
    # 与y轴夹角，用于判断是否在探测器窗口内
    direction_y = np.arccos(direction[1])
    # 投影到xz平面与z轴夹角，用于判断是否在探测器窗口内
    direction_z_xz = np.arctan2(direction[0], direction[2])
    # 与z轴夹角，用于记录通过探测器窗口的角度
    direction_z = np.arccos(direction[2])
    if (
        ydthetamin <= direction_y <= ydthetamax
        and zdthetamin <= direction_z_xz <= zdthetamax
    ):
        return direction_z
    return None


# 收集通过探测窗口的粒子的角度
def collect_angles(sigma, w, h, rd, theta_c, num_samples=1000):
    angles = []
    for _ in range(num_samples):
        angle = is_in_window(sigma, w, h, rd, theta_c)
        if angle is not None:
            angles.append(angle)
    return angles


# 进行模拟
angles = collect_angles(sigma, w, h, rd, theta_c, n_particles)

# 将角弧度转换为角度
angles_in_degrees = np.rad2deg(angles)

# 计算角度的频率分布
hist, bin_edges = np.histogram(angles_in_degrees, bins=30, density=True)

# 计算等效角度
bin_centers = (bin_edges[:-1] + bin_edges[1:]) / 2
# equivalent_angle = np.sum(hist * bin_centers * np.diff(bin_edges))
equivalent_angles = np.sum(hist * bin_centers) / np.sum(hist)

n_out = len(angles)  # 通过探测窗口的粒子数目
solid_angle = n_out / n_particles * 4 * np.pi # 计算立体角
solid_angle_ratio = n_out / n_particles

# print(f"等效角度: {equivalent_angle} 度")
print(f"等效角度: {equivalent_angles} 度")
print(f"通过探测窗口的粒子数目: {n_out}")
print(f"立体角: {solid_angle} sr")
print(f"立体角占比: {solid_angle_ratio * 100:.2f}%")

# 绘制角度出现频率直方图
plt.figure(figsize=(8, 6))
plt.hist(angles_in_degrees, bins=30, edgecolor="black", alpha=0.7)
plt.xlabel("Angle (degrees)")
plt.ylabel("Frequency")
plt.title("Histogram of Angles Passing Through the Detection Window")
plt.grid(True)
plt.show()
