import gym
import numpy as np
import sys
import frankwolfe as fw
from gym.spaces import Discrete, Box
from ray.rllib.env.env_context import EnvContext
import tensorflow as tf

class TrafficEnv(gym.Env):
	"""Example of a custom env in for traffic"""

	def __init__(self, config: EnvContext):
		self.demand=config["demand"]
		self.initial_perturbed=config["perturbed"]
		self.perturbed=self.initial_perturbed.copy()
		self.initial_state=config["fake_flow"] #[f0,f1,f2...]
		self.state=self.initial_state.copy()
		self.real_flow=config["real_flow"] #[f0,f1,f2...]
		self.edges_count=len(self.state)

		self.fw_iterations=config["fw_iterations"]
		self.reg_beta=config["reg_beta"]

		self.episode_ended = False
		self.step_count=0
		self.horizon=config['horizon']

		self.space_size=len(self.state)

		self.action_space = Box(np.array([0.0 for _ in range(self.space_size)]),np.array([1.0 for _ in range(self.space_size)]),dtype=np.float32)
		self.observation_space =Box(np.array([0.0 for _ in range(self.space_size)]),np.array([sys.float_info.max for _ in range(self.space_size)]),dtype=np.float32)
		

	def reset(self):
		self.state = self.initial_state.copy()
		self.perturbed=self.initial_perturbed.copy()
		self.episode_ended = False
		self.step_count=0
		#self.observation_space =Box(np.array([0.0 for _ in range(self.space_size)]),np.array([sys.maxsize for _ in range(self.space_size)]))
		return self.state

	def step(self, action):
		self.step_count+=1
		self.episode_ended = self.step_count >= self.horizon

		'''pairs=fw.importODPairsFrom(self.demand)
		graph=fw.Graph(self.perturbed,0,100)
		assign=fw.FrankWolfeAssignment(graph,pairs,True,False)

		self.state=assign.runPython(100)'''
		atp=fw.AssignTrafficPython()
		self.state=np.cast["float32"](atp.flow(self.demand,self.perturbed,self.fw_iterations)["flow"])
		diff=[]
		for a,b in zip(self.real_flow,self.state):
			diff.append(np.abs(a-b))
		if self.episode_ended is False:
			for x in range(len(action)):
				self.perturbed["capacity"][x]=int(1000*action[x])+50
		#beta=0.1 #coefficient on regularization term
		reg=self.reg_beta*np.linalg.norm(self.perturbed["capacity"])
		reward=-(np.linalg.norm(diff)+reg)
		return self.state, reward, self.episode_ended, {}


class InvertedTrafficEnv(gym.Env):

	def __init__(self,config: EnvContext):
		self.edges=config["edges"]
		self.initial_demand=config["initial_demand"]
		self.current_demand=self.initial_demand.copy()
