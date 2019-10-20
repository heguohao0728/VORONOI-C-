#include "ofxVoronoi.h"

// Voro++2D
#include "config.h"
#include "common.h"
#include "cell_2d.h"
#include "v_base_2d.h"
#include "rad_option.h"
#include "container_2d.h"
#include "v_compute_2d.h"
#include "c_loops_2d.h"
#include "wall_2d.h"
#include "cell_nc_2d.h"
#include "ctr_boundary_2d.h"

//--------------------------------------------------------------
void ofxVoronoi::clear() {
    cells.clear();
    points.clear();
}

//--------------------------------------------------------------
void ofxVoronoi::generate(bool ordered) {
    voro::container_2d* con = new voro::container_2d(bounds.x, bounds.x+bounds.getWidth(), bounds.y, bounds.y+bounds.getHeight(), 10, 10, false, false, 16);
    voro::c_loop_all_2d* vl = new voro::c_loop_all_2d(*con);
    voro::voronoicell_2d conCell;
        
    for(int i=0; i<points.size(); i++) {
        con->put(i, points[i].x, points[i].y);
    }
    
    if(vl->start()) {
        do {
            con->compute_cell(conCell, *vl);
            int k = 0;
            
            if(conCell.p) {
                ofxVoronoiCell newCell;
                
                // Get the current point of the cell
				double* currentPoint = con->p[vl->ij] + con->ps*vl->q;
                newCell.centroid = ofDefaultVec3(currentPoint[0], currentPoint[1], 0);
                
                // Get the edgepoints of the cell
                do {
                    float x = currentPoint[0] + 0.5 * conCell.pts[2*k];
                    float y = currentPoint[1] + 0.5 * conCell.pts[2*k+1];
                    
                    ofDefaultVec3 pt = ofDefaultVec3(x, y, 0);
                    newCell.points.push_back(pt);
                    
                    k = conCell.ed[2*k];
                } while(k!=0);
                
                cells.push_back(newCell);
            }
        } while(vl->inc());
    }
    
    // free up the memory
    delete con, vl;
    
    if(ordered) {
        std::vector<ofxVoronoiCell> orderedCells;
        for(auto& p : points) {
            // ofLog() << p;
            orderedCells.push_back(getCell(p));
        }
        cells = orderedCells;
    }
}

//--------------------------------------------------------------
void ofxVoronoi::draw() {
    
    ofPushStyle();
    
    ofSetLineWidth(0);
    
    // Draw cells
    ofFill();

    for(int i=0; i<cells.size(); i++) {
        // Draw cell borders
        ofSetColor(220, 220, 220, 180);
        for(int j=0; j<cells[i].points.size(); j++) {
            size_t p = (j+1) % cells[i].points.size();
            ofDrawLine(cells[i].points[p], cells[i].points[j]);
        }
        // Draw cell points
        ofSetColor(180, 0, 0, 180);
        ofDrawCircle(cells[i].centroid, 2);
    }

    // Draw bounds
    ofSetColor(220, 0, 0, 180);
    ofNoFill();
    ofDrawRectangle(bounds);

    ofPopStyle();
}

//--------------------------------------------------------------
void ofxVoronoi::setBounds(ofRectangle _bounds) {
    bounds = _bounds;
}

//--------------------------------------------------------------
void ofxVoronoi::setPoints(std::vector<ofDefaultVec3> _points) {
    clear();
    points = _points;
}

//--------------------------------------------------------------
void ofxVoronoi::addPoint(ofDefaultVec3 _point) {
    points.push_back(_point);
}

//--------------------------------------------------------------
void ofxVoronoi::addPoints(std::vector<ofDefaultVec3> _points) {
    points.insert( points.end(), _points.begin(), _points.end() );
}

//--------------------------------------------------------------
ofRectangle ofxVoronoi::getBounds() {
    return bounds;
}

//--------------------------------------------------------------
std::vector<ofDefaultVec3>& ofxVoronoi::getPoints() {
    return points;
}

//--------------------------------------------------------------
std::vector<ofxVoronoiCell>& ofxVoronoi::getCells() {
    return cells;
}

//--------------------------------------------------------------
void ofxVoronoi::relax(){
    std::vector<ofDefaultVec3> relaxPts;
    for(int i=0; i<cells.size(); i++) {
        ofPolyline line;
        for (auto p:cells[i].points){
            line.addVertex(p.x, p.y);
        }
        line.close();
        ofDefaultVec3 centroid = line.getCentroid2D();
        relaxPts.push_back(centroid);
    }
    clear();
    points = relaxPts;
    generate();
}
//--------------------------------------------------------------
/*好像暂时没用上
std::vector<ofDefaultVec3>& ofxVoronoi::cal_center(ofxVoronoiCell cell)
{
	// TODO: 在此处插入 return 语句
}
*/
//--------------------------------------------------------------
void ofxVoronoi::cal_neighbor()
{
	//我们将cell 1,4,7,10号设置为追踪者
	int i, j, k, l;
	double xm, ym;
	struct linepoint {
		std::vector<ofDefaultVec3> commonpoint;
	}commonpoints;
	struct centroidpoint {
		std::vector<ofDefaultVec3> centroidpoint;
	}centroidpoints;
	for (i = 0; i < cells.size(); i++) {
		for (j = 0; j < cells.size(); j++) {
			if (i != j) {
				//TODO：这一块可以继续融合成一个函数来调用，写在这里太冗长
				int m = 0;
				for (k = 0; k < cells[i].points.size(); k++) {
					for (l = 0; l < cells[j].points.size(); l++) {
						if (cells[i].points[k] == cells[j].points[l]) {
							m++;
							commonpoints.commonpoint.push_back(cells[j].points[l]);
							break;
						}
					}
					if (m == 2) break;
				}
				if (m == 2) {
					//相邻元胞索引
					cells[i].neighborindex.push_back(j);
					cells[i].neighborcentroidpoints.push_back(cells[j].centroid);
					xm = (commonpoints.commonpoint[0].x + commonpoints.commonpoint[1].x) / 2;
					ym = (commonpoints.commonpoint[0].y + commonpoints.commonpoint[1].y) / 2;
					/*cells[i].neighbormidpoints[0][0] = xm;
					cells[i].neighbormidpoints[0][1] = ym;
					cells[i].neighbormidpoints[0][2] = 0;*/
					ofDefaultVec3 pt = ofDefaultVec3(xm, ym, 0);
					cells[i].neighbormidpoints.push_back(pt);
					commonpoints.commonpoint.clear();
				}
				//--------------------------------------------------------
			}
		}
	}
}

//--------------------------------------------------------------
void ofxVoronoi::cal_up()
{
	int i, j, p;                   
	for (i = 1; i < 12; i = i + 3) {
		double min_dis_with_cell = 10000;
		for (j = 0; j < cells[i].neighborindex.size(); j++) {			
			//相邻的元胞要是逃脱者
			if (cells[i].neighborindex[j] != 1 && cells[i].neighborindex[j] != 4 && cells[i].neighborindex[j] != 7 && cells[i].neighborindex[j] != 10) {
				//min_dis_with_cell = cells[i].dis_with_cell;
				cells[i].dis_with_cell = sqrt(pow(cells[i].neighborcentroidpoints[j].x - cells[i].centroid.x, 2) +
					pow(cells[i].neighborcentroidpoints[j].y - cells[i].centroid.y, 2));
				if (cells[i].dis_with_cell < min_dis_with_cell){
					p = j;      //找到最近元胞，编号为p
					min_dis_with_cell = cells[i].dis_with_cell;
				}
			}//这里有问题，如果所有的临近元胞都不是逃脱者，这个up算不出来（即还没有写当没有临近逃脱者元胞时，使用最近的逃脱者作为目标的方法）
		}
		cells[i].dis_with_cell = min_dis_with_cell;    //把距离赋值为距离最近的那个点的距离大小
		//x方向速度
		cells[i].up.x=(cells[i].neighbormidpoints[p].x-cells[i].centroid.x)/
			(sqrt(pow(cells[i].neighbormidpoints[p].x - cells[i].centroid.x, 2) +
				pow(cells[i].neighbormidpoints[p].y - cells[i].centroid.y, 2)));
		//y方向速度
		cells[i].up.y = (cells[i].neighbormidpoints[p].y - cells[i].centroid.y) /
			(sqrt(pow(cells[i].neighbormidpoints[p].x - cells[i].centroid.x, 2) +
				pow(cells[i].neighbormidpoints[p].y - cells[i].centroid.y, 2)));
		cells[i].up.z = 0;
	}
}

//--------------------------------------------------------------
void ofxVoronoi::update_point()
{
	// TODO: 在此处插入 return 语句
	double timestep = 1;
	int i;
	points.clear();
	for(i = 0; i < cells.size(); i++) {
		cells[i].centroid.x = cells[i].centroid.x + cells[i].up.x*timestep;
		cells[i].centroid.y = cells[i].centroid.y + cells[i].up.y*timestep;
		points.push_back(cells[i].centroid);
	}
	//ofxVoronoi::generate();
	for (i = 0; i < cells.size(); i++) {
		cells[i].neighborindex.clear();
		cells[i].neighborcentroidpoints.clear();
		cells[i].neighbormidpoints.clear();
	}
}

void ofxVoronoi::catchornot()
{
	int i,j;
	double dis;
	//只对evader判断是否捕捉
	for (i = 0; i < 12 && i != 1 && i != 4 && i != 7 && i != 10; i++) {
		if (cells[i].alive == 1) {
			for (j = 1; j < 12; j = j + 3) {
				dis = sqrt(pow(cells[j].centroid.x - cells[i].centroid.x, 2) + pow(cells[j].centroid.y - cells[i].centroid.y, 2));
				if (dis < cells[i].dis_with_cell) {
					cells[i].dis_with_cell = dis;
				}
			}
			if (cells[i].dis_with_cell < cells[i].capure_dis) {
				cells[i].alive = 0;
				for (j = 1; j < 12; j = j + 3) {
					cells[j].dis_with_cell = 10000;
				}
			}
		}
	}
	return ;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
//--------------------------------------------------------------
ofxVoronoiCell& ofxVoronoi::getCell(ofDefaultVec3 _point, bool approximate) {
    if(approximate) {
        ofxVoronoiCell& nearestCell = cells[0];
        float nearestDistance = numeric_limits<float>::infinity();
        for(ofxVoronoiCell& cell : cells) {
            float distance = getDistance(_point, cell.centroid);
            if(distance < nearestDistance) {
                nearestDistance = distance;
                nearestCell = cell;
            }
        }
        return nearestCell;
    } else {
        for(ofxVoronoiCell& cell : cells) {
            if(_point == cell.centroid) {
                return cell;
            }
        }
        ofLogError("ofxVoronoi") << "getCell could not find exact match for " << _point;
        return cells[0];
    }
}

float ofxVoronoi::getDistance(ofPoint p1, ofPoint p2){
    return p1.squareDistance(p2);
}
float ofxVoronoi::getDistance(glm::vec3 p1, glm::vec3 p2){
    return glm::distance2(p1, p2);
}

