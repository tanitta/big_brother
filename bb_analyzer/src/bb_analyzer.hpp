#pragma once

#include <ofMain.h>
#include <pharticle/pharticle.hpp>
#include <data_manager.hpp>
#include <user.hpp>
namespace big_brother {
	class BBAnalyzer : public ofBaseApp{
		private:
			pharticle::Engine ph_engine_;
			DataManager data_manager_;
			
			ofEasyCam cam_;
		public:
			BBAnalyzer(){};
			virtual ~BBAnalyzer(){};
			void setup(){
				//graphics
				ofEnableAlphaBlending();
				ofEnableDepthTest();
				ofEnableAntiAliasing();
				ofBackground(32,32,32);
				
				data_manager_.load_from_xml("hoge.xml");
				data_manager_.setup_particle();
				
				ph_engine_.set_func_reaction_force([](pharticle::Particle& p1, pharticle::Particle& p2){
					Eigen::Vector3d f(0,0,0);
					Eigen::Vector3d p(0,0,0);
					p = (p2.position_ - p1.position_);
					
					double r = p1.radius_ + p2.radius_;
					Eigen::Vector3d v_r = r*p.normalized();
					f = (v_r - p)*0.5;
					f +=(p1.velocity_ - p2.velocity_)*0.1;
					// p = (p2.position_ - p1.position_)*p2.mass_*0.01*v.norm();
					return f;
				});
				
				auto func_spring = [=](pharticle::Particle& p1, pharticle::Particle& p2){
					Eigen::Vector3d v(0,0,0);
					v = (p2.position_ - p1.position_);
					Eigen::Vector3d nv = v.normalized();
					v = (nv*(120) - v)*0.5;
					v += (p1.velocity_ - p2.velocity_)*0.5;
					v +=(- p2.velocity_)*0.5;
					return v;
				};
				
				for (auto&& user : data_manager_.users_) {
					ph_engine_.add_particle_ptr(user.second.particle_);
				}
				
				data_manager_.generate_relation(func_spring);
				
				data_manager_.print();
				std::cout << data_manager_.relations_.size() << std::endl;
				
				data_manager_.randomize(-200,200);
			};
			
			void update(){
				auto func_diffusion = [=](pharticle::Particle& p1, pharticle::Particle& p2){
					Eigen::Vector3d v(0,0,0);
					v = (p2.position_ );
					// v.normalize();
					Eigen::Vector3d f(0,0,0);
					if(v.norm() != 0){
						v.normalize();
						f = v*10;
					}
					// Eigen::Vector3d nv = v.normalized();
					// v = 0;
					// v +=(p1.velocity_ - p2.velocity_)*0.5;
					// v +=(- p2.velocity_)*0.2;
					return f;
				};
				for (auto&& user : data_manager_.users_) {
					ph_engine_.add_constraint_pair(&data_manager_.users_[data_manager_.entry_id_].particle_,&user.second.particle_,func_diffusion);
				}
				
				for (auto&& pair : data_manager_.relations_) {
					ph_engine_.add_constraint_pair_bdi(pair);
				}
				ph_engine_.update();
				
				data_manager_.users_[data_manager_.entry_id_].particle_.position_[0] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.position_[1] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.position_[2] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.velocity_[0] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.velocity_[1] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.velocity_[2] = 0;
				
				
				// int n = mesh.getNumVertices();
				// float nearestDistance = 0;
				// ofVec2f nearestVertex;
				// int nearestIndex = 0;
				// ofVec2f mouse(mouseX, mouseY);
				// for(int i = 0; i < n; i++) {
				// 	ofVec3f cur = cam.worldToScreen(mesh.getVertex(i));
				// 	float distance = cur.distance(mouse);
				// 	if(i == 0 || distance < nearestDistance) {
				// 		nearestDistance = distance;
				// 		nearestVertex = cur;
				// 		nearestIndex = i;
				// 	}
				// }
			};
			
			void draw(){
				cam_.begin();
				ofFill();
				ofSetColor(255,255,255);
				for (auto&& user : data_manager_.users_) {
					double x = user.second.particle_.position_[0];
					double y = user.second.particle_.position_[1];
					double z = user.second.particle_.position_[2];
					ofSphere(x,y,z,10);
					
				}
				
				for (auto&& pair : data_manager_.relations_) {
					double x1 = pair.particle_ptrs_[0]->position_[0];
					double y1 = pair.particle_ptrs_[0]->position_[1];
					double z1 = pair.particle_ptrs_[0]->position_[2];
					
					double x2 = pair.particle_ptrs_[1]->position_[0];
					double y2 = pair.particle_ptrs_[1]->position_[1];
					double z2 = pair.particle_ptrs_[1]->position_[2];
					
					ofLine(x1,y1,z1,x2,y2,z2);
				}
				cam_.end();
				
				double range = 100;
				double nearest_distance = range;
				ofVec2f nearest_vertex;
				int nearest_id = 0;
				ofVec2f mouse(mouseX, mouseY);
				for(auto&& user : data_manager_.users_){
					double x = user.second.particle_.position_[0];
					double y = user.second.particle_.position_[1];
					double z = user.second.particle_.position_[2];
					ofVec3f cur = cam_.worldToScreen(ofVec3f(x,y,z));
					// ofSetColor(ofColor::white);
					// ofFill();
					// ofCircle(cur, 2);
					double distance = cur.distance(mouse);
					if(distance < nearest_distance) {
						nearest_distance= distance;
						nearest_vertex = cur;
						nearest_id = user.second.id_;
					}
				}
				// std::cout << "nearest_distance: " << nearest_distance<< std::endl;
				if(nearest_distance < range){
					ofSetColor(ofColor::gray);
					ofLine(nearest_vertex, mouse);

					ofNoFill();
					ofSetColor(ofColor::magenta);
					ofSetLineWidth(2);
					ofCircle(nearest_vertex, 4);
					ofSetLineWidth(1);

					ofVec2f offset(10, -10);
					std::string info = "";
					info +="user_id : "+ofToString(nearest_id);
					info +="\n";
					info +="screen_name : "+ofToString(data_manager_.users_[nearest_id].screen_name_);
					ofDrawBitmapStringHighlight(info, mouse + offset);
				}
			};
			
			void keyPressed(int key){};
			void keyReleased(int key){};
			void mouseMoved(int x, int y ){};
			void mouseDragged(int x, int y, int button){};
			void mousePressed(int x, int y, int button){};
			void mouseReleased(int x, int y, int button){};
			void windowResized(int w, int h){};
			void dragEvent(ofDragInfo dragInfo){};
			void gotMessage(ofMessage msg){};
	};
} // namespace big_brother
