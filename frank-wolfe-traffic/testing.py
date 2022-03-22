import example
import timeit
import numpy as np
from random import randint, choice,sample

edges={}
demand={}
nodes=1000
commuters=set()
paths=set()
for z in range(5000):
    x=randint(0,nodes)
    y=randint(0,nodes)
    while x==y or (x,y) in commuters:
        y=randint(0,nodes)
        x=randint(0,nodes)
    commuters.add((x,y))
    stops=randint(1,3)
    intermediate=[z for z in range(nodes) if z!=x and z!=y]
    path=(x,* sample(intermediate,stops),y)
    while path in paths:
        path=(x,* sample(intermediate,stops),y)
    paths.add(path)
    for p in range(len(path)-1):
        paths.add((path[p],path[p+1]))



for key in ["edge_tail", "edge_head", "length", "capacity", "speed"]:
    edges[key]=[]

for key in ["origin","destination","volume"]:
    demand[key]=[]

for (t,h) in commuters:
    demand["origin"].append(t)
    demand["destination"].append(h)
    demand["volume"].append(randint(20,100))

for p in sorted(paths):
    if len(p)>2:
        continue
    (t,h)=p
    edges["edge_tail"].append(t)
    edges["edge_head"].append(h)
    edges["length"].append(randint(5,10))
    edges["capacity"].append(randint(20,100))
    edges["speed"].append(randint(25,70))

#print(example.flow(demand,edges))

times=timeit.repeat(stmt="example.flow(demand,edges)",number=1,globals=globals(),repeat=30)
print("min",round(min(times),4),"max",round(max(times),4),"mean",round(np.mean(times),4),"std",round(np.std(times),4))