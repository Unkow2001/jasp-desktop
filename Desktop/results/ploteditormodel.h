#ifndef PLOTEDITORMODEL_H
#define PLOTEDITORMODEL_H

#include <QUrl>
#include <QObject>
#include "jsonredirect.h"
#include "ploteditoraxismodel.h"
#include "ploteditorcoordinates.h"
#include <stack>

class Analyses;
class Analysis;

namespace PlotEditor
{

///
/// Main model for the ploteditor, loads the `editingOptions` from `writeImage` and initializes itself with that.
/// Whenever changes are made these are communicated to the right engine and the results caught and processed here again
/// It also makes sure that the Analysis is updated.
class PlotEditorModel : public QObject
{
	Q_OBJECT
	Q_ENUMS(AxisType)

	Q_PROPERTY(bool						visible			READ visible		WRITE setVisible		NOTIFY visibleChanged			)
	Q_PROPERTY(QString					name			READ name			WRITE setName			NOTIFY nameChanged				)
	Q_PROPERTY(QString					data			READ data			WRITE setData			NOTIFY dataChanged				)
	Q_PROPERTY(QUrl						imgFile			READ imgFile								NOTIFY dataChanged				)
	Q_PROPERTY(QString					title			READ title			WRITE setTitle			NOTIFY titleChanged				)
	Q_PROPERTY(int						width			READ width			WRITE setWidth			NOTIFY widthChanged				)
	Q_PROPERTY(int						height			READ height			WRITE setHeight			NOTIFY heightChanged			)
	Q_PROPERTY(AxisModel *				xAxis			READ xAxis									NOTIFY dummyAxisChanged			)
	Q_PROPERTY(AxisModel *				yAxis			READ yAxis									NOTIFY dummyAxisChanged			)
	Q_PROPERTY(double					ppi				READ ppi									NOTIFY ppiChanged				)
	Q_PROPERTY(bool						loading			READ loading		WRITE setLoading		NOTIFY loadingChanged			)
	Q_PROPERTY(bool						undoEnabled		READ undoEnabled							NOTIFY unOrRedoEnabledChanged	)
	Q_PROPERTY(bool						redoEnabled		READ redoEnabled							NOTIFY unOrRedoEnabledChanged	)
	Q_PROPERTY(AxisModel *				currentAxis		READ currentAxis							NOTIFY currentAxisChanged		)
	Q_PROPERTY(AxisType					axisType		READ axisType		WRITE setAxisType		NOTIFY axisTypeChanged			)

public:
	explicit PlotEditorModel();

	enum class AxisType  { Xaxis, Yaxis }; // add right axis, top axis, etc.

	struct undoRedoData
	{
		AxisType			currentAxis;
		Json::Value			options;
	};

	bool					visible()	const {	return _visible;	}
	QString					name()		const { return _name;		}
	QString					data()		const { return _data;		}
	QUrl					imgFile()	const;
	QString					title()		const { return _title;		}
	int						width()		const { return _width;		}
	int						height()	const { return _height;		}
	AxisModel			*	xAxis()		const { return _xAxis;		}
	AxisModel			*	yAxis()		const { return _yAxis;		}
	double					ppi()		const {	return _ppi;		}
	bool					loading()	const { return _loading;	}
	void					reset();

	bool					undoEnabled()	const {	return _undo.size() > 0;	}
	bool					redoEnabled()	const {	return _redo.size() > 0;	}

	AxisModel			*	currentAxis()	const {	return _currentAxis;	}
	AxisType				axisType()		const { return _axisType;		}

	void					setBlockChanges(bool change) { _blockChanges = change; }
	bool					blockChanges()	const { return _blockChanges;	}

signals:
	void visibleChanged(		bool		visible			);
	void nameChanged(			QString		name			);
	void dataChanged(										);
	void titleChanged(			QString		title			);
	void widthChanged(			int			width			);
	void heightChanged(			int			height			);
	void dummyAxisChanged();
	void ppiChanged();// TODO, refresh all
	void resetPlotChanged(		bool		resetPlot		);
	void loadingChanged(		bool		loading			);
	void unOrRedoEnabledChanged();
	

	void saveImage(int	id,	QString		options)	const;

	void currentAxisChanged(	AxisModel * currentAxis);
	void axisTypeChanged(		AxisType	axisType);

public slots:
	void showPlotEditor(int id, QString options);
	void updateOptions(Analysis* analysis);

	void setVisible(		bool					visible			);
	void setName(			const QString	&		name			);
	void setData(			const QString	&		data			);
	void setTitle(			const QString	&		title			);
	void setWidth(			int						width			);
	void setHeight(			int						height			);
	void setLoading(		bool					loading			);
	
	void					resetDefaults();
	void					cancelPlot();
	void					savePlot()	const;
	void setAxisType(		const AxisType			axisType		);

	void somethingChanged();
	void refresh();

	//QString clickHitsElement(double x, double y) const;

	void addToUndoStack();

	void undoSomething(); //No need to do Q_INVOKABLE for slots, they are always available from QML
	void redoSomething();
	void applyChangesFromUndoOrRedo(const undoRedoData& newData);


private:
	void		setup();
	Json::Value generateImgOptions()	const;
	void		updatePlot(Json::Value& imageOptions);

private:
	Analysis			*	_analysis		= nullptr;
	AxisModel			*	_xAxis			= nullptr,
						*	_yAxis			= nullptr,
						*	_currentAxis	= nullptr;

	//Coordinates				_coordinates;
	Json::Value				_imgOptions		= Json::nullValue,
							_originalImgOps	= Json::nullValue;
	std::map<int, Json::Value> _editedImgsMap;
	QString					_name,
							_data,
							_title;
	bool					_visible		= false,
							_goBlank		= false,
							_loading		= false,
							_validOptions	= false,
							_blockChanges	= false;
	int						_width,
							_height;
	double					_ppi;

	static int				_editRequest;


	std::stack<undoRedoData>	_undo,
								_redo;

	AxisType				_axisType		= AxisType::Xaxis;
};

}

#endif // PLOTEDITORMODEL_H

