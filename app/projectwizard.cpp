#include "projectwizard.h"

ProjectWizard::ProjectWizard()
{

}

void ProjectWizard::createProject()
{

    QString projectName = QString("TODO"); // TODO;
    // Create a new folder for a project


    QString fileName( mDatabase->filePath() );
      if ( !fileName.endsWith( QLatin1String( ".gpkg" ), Qt::CaseInsensitive ) )
        fileName += QLatin1String( ".gpkg" );

}
