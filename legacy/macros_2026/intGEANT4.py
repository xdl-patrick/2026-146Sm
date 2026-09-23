import numpy as np
import matplotlib.pyplot as plt

# ================= 参数设置 =================
source_radius = 7.5      # 放射源半径 (mm)，直径1.5cm = 15mm
detector_size = 50.0     # 正方形硅探测器边长 (mm)
n_particles = 1000000    # 模拟粒子总数

# 在此处修改距离：23mm 或 93mm
rd = 93.0                

# ================= 采样函数 =================
# 1. 在圆形放射源内均匀采样一点
def sample_point_in_circle(radius):
    # 使用平方根变换保证圆内面积均匀分布
    r = radius * np.sqrt(np.random.uniform(0, 1))
    theta = np.random.uniform(0, 2 * np.pi)
    x = r * np.cos(theta)
    y = r * np.sin(theta)
    return x, y

# 2. 抽取一个各向同性的随机方向单位向量
def sample_direction():
    theta = np.random.uniform(0, 2 * np.pi)
    cospsi = np.random.uniform(-1, 1)
    psi = np.arccos(cospsi)
    return np.array([
        np.sin(psi) * np.cos(theta), 
        np.sin(psi) * np.sin(theta), 
        np.cos(psi)
    ])

# ================= 几何判断 =================
# 判断粒子是否击中边长为 size 的正方形探测器
def is_hit_detector(source_radius, detector_size, rd, num_samples):
    hit_count = 0
    half_size = detector_size / 2.0
    
    for _ in range(num_samples):
        # 获取发射点 (x, y) 和方向向量 (dx, dy, dz)
        x, y = sample_point_in_circle(source_radius)
        dx, dy, dz = sample_direction()
        
        # 仅考虑向前发射的粒子 (dz > 0)
        if dz <= 0:
            continue
            
        # 计算粒子到达探测器平面 (z = rd) 时的坐标
        # 参数 t 满足: z + t * dz = rd => t = rd / dz
        t = rd / dz
        hit_x = x + t * dx
        hit_y = y + t * dy
        
        # 判断是否落在正方形探测器范围内
        if -half_size <= hit_x <= half_size and -half_size <= hit_y <= half_size:
            hit_count += 1
            
    return hit_count

# ================= 执行模拟与结果输出 =================
n_out = is_hit_detector(source_radius, detector_size, rd, n_particles)

solid_angle_ratio = n_out / n_particles
solid_angle = solid_angle_ratio * 4 * np.pi  # 单位: 球面度 (sr)

print(f"--- 模拟结果 (距离 rd = {rd} mm) ---")
print(f"总模拟粒子数: {n_particles}")
print(f"击中探测器的粒子数: {n_out}")
print(f"立体角占比: {solid_angle_ratio * 100:.4f}%")
print(f"计算得到的立体角: {solid_angle:.6f} sr")