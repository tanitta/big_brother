#pragma once

#include <ofxXmlSettings.h>
#include <user.hpp>
#include <pharticle/pharticle.hpp>
//! 可視化するデータを取り扱う
/*!
 * detail
 */
namespace big_brother {
	class DataManager {
		private:
		public:
			std::map<int,User> users_;
			std::vector<pharticle::ConstraintPair> relations_;
			int entry_id_;
			
			DataManager():entry_id_(0){};
			virtual ~DataManager(){};
			
			void load_from_xml(std::string name){
				ofxXmlSettings xml;
				std::string message;
				if(xml.loadFile(name)){
					message = name+" loaded!";
				}else{
					message = "unable to load "+name+" check data/ folder";
				}
				std::cout<<message<<std::endl;
				
				for(int i = 0; i < xml.getNumTags("user"); i++ ){
					
					xml.pushTag("user", i);
					int id = xml.getValue("id", 0);
					if(i == 0){
						entry_id_ = id;
					}
					if(users_.find(id) == users_.end()){
						//キーが存在しない
						std::string screen_name = xml.getValue("screen_name","");
						add_user(id,screen_name);
						xml.pushTag("followers");
						for(int j = 0; j < xml.getNumTags("id"); j++ ){
							int follower_id = xml.getValue("id", 0, j);
							if(users_.find(follower_id) == users_.end()){
							add_user(follower_id,"??");
							}
							users_[id].followers_.insert(follower_id);
						}
						xml.popTag();
						
					}else{
						//キーが存在する
						xml.pushTag("followers");
						for(int j = 0; j < xml.getNumTags("id"); j++ ){
							int follower_id = xml.getValue("id", 0, j);
							if(users_.find(follower_id) == users_.end()){
							add_user(follower_id,"??");
							}
							users_[id].followers_.insert(follower_id);
						}
						xml.popTag();
					}
					xml.popTag();
				}
			};
			
			void randomize(double min,double max){
				for (auto&& user : users_) {
					user.second.particle_.position_[0] = ofRandom(min,max);
					user.second.particle_.position_[1] = ofRandom(min,max);
					user.second.particle_.position_[2] = ofRandom(min,max);
				}
			}
			
			void setup_particle(){
				for (auto&& user : users_) {
					user.second.setup_particle();
				}
			};
			
			void print(){
				for (auto&& user : users_) {
					user.second.print();
				}
			};
			
			// auto func_spring = [](pharticle::Particle& p1, pharticle::Particle& p2){
			// 	Eigen::Vector3d v(0,0,0);
			// 	v = (p2.position_ - p1.position_);
			// 	Eigen::Vector3d nv = v.normalized();
			// 	v = (nv*(200) - v)*1;
			// 	v +=(p1.velocity_ - p2.velocity_)*0.5;
			// 	return v;
			// };
			
			void generate_relation(std::function<Eigen::Vector3d(pharticle::Particle&, pharticle::Particle&)> func){
				for (auto&& user : users_) {
					for (auto&& follower_id : user.second.followers_) {
						relations_.push_back(pharticle::ConstraintPair(&user.second.particle_, &users_[follower_id].particle_,func));
					}
				}
			}
			
			void add_user(int id, std::string screen_name){
				User tmp_user;
				tmp_user.id_ = id;
				tmp_user.screen_name_ = screen_name;
				users_[id] = tmp_user;
			}
	};
} // namespace big_brother
