#pragma once
#include <pharticle/pharticle.hpp>
namespace big_brother {
	class User {
		public:
			int id_;
			std::string screen_name_;
			std::set<int> followers_;
			
			pharticle::Particle particle_;
			
			User():particle_(){};
			virtual ~User(){};
			
			void setup_particle(){
				particle_.id_ = id_;
				particle_.mass_ = (1+followers_.size())*10;
				particle_.radius_= 40.0;
			};
			
			void print(){
				std::cout << "id : " << id_ << std::endl;
				std::cout << "screen_name : " << screen_name_ << std::endl;
				std::cout << "followers : " << followers_.size() << std::endl;
				for (auto&& p : followers_) {
					std::cout << "\tid : " << p << std::endl;
				}
			};
	};
} // namespace big_brother
